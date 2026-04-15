#include <views/MusicView.hpp>
#include <widgets/Button.hpp>
#include <views/MainView.hpp>
#include <cmath>
#include <iostream>

MusicView::MusicView() {}

MusicView::~MusicView() {}

void MusicView::start() {
    _font = LoadFontEx(_fontPath.c_str(), 64, nullptr, 0);

    auto backBtn = std::make_unique<Button>(W - 168, H - 52, 160, 36, "BACK");
    backBtn->setOnClick([this]() {
        ViewHandler::getInstance().switchView(std::make_unique<MainView>());
    });
    _widgets.push_back(std::move(backBtn));

    // Init PulseAudio monitor source
    pa_sample_spec spec;
    spec.format   = PA_SAMPLE_FLOAT32LE;
    spec.channels = 1;
    spec.rate     = kSampleRate;

    int error;
    _pulse = pa_simple_new(
        nullptr,                    // default server
        "PiPOC",                    // app name
        PA_STREAM_RECORD,
        "alsa_output.platform-bcm2835_audio.analog-stereo.monitor",  // Pi monitor source
        "visualizer",               // stream name
        &spec,
        nullptr,                    // default channel map
        nullptr,                    // default buffering
        &error
    );

    if (!_pulse) {
        std::cerr << "[VIS] PulseAudio error: " << pa_strerror(error) << "\n";
    }

    // Init KissFFT
    _fftCfg = kiss_fftr_alloc(kBufferSize, 0, nullptr, nullptr);

    // Start capture thread
    _capturing = true;
    _captureThread = std::jthread([this](std::stop_token st) { captureLoop(); });
}

void MusicView::close() {
    _capturing = false;
    _captureThread.request_stop();

    // Wait for capture thread to finish before freeing pulse
    if (_captureThread.joinable()) _captureThread.join();

    if (_pulse) {
        pa_simple_free(_pulse);
        _pulse = nullptr;
    }
    if (_fftCfg) {
        kiss_fftr_free(_fftCfg);
        _fftCfg = nullptr;
    }

    UnloadFont(_font);
}

void MusicView::draw() {
    Color bg        = {10,  10,  10,  255};
    Color barColor  = {39,  174, 96,  255};  // green placeholder
    Color dimText   = {120, 120, 120, 255};
    Color debugColor= {80,  80,  80,  255};

    DrawRectangle(0, 0, W, H, bg);

    // Visualizer
    const int barCount  = 48;
    const int barW      = 16;
    const int barGap    = 4;
    const int totalBarW = barCount * (barW + barGap);
    const int barStartX = (W - totalBarW) / 2;
    const int barBaseY  = (int)(H * 0.62f);
    const int barMaxH   = (int)(H * 0.55f);

    for (int i = 0; i < kBarCount; i++) {
        int bh = (int)(_barHeights[i] * barMaxH);
        bh = std::max(2, bh);  // minimum bar height so they're always visible
        int bx = barStartX + i * (barW + barGap);
        int by = barBaseY - bh;
        DrawRectangle(bx, by, barW, bh, barColor);
    }

    // Album art placeholder box
    DrawRectangle(20, H - 160, 120, 120, {30, 30, 30, 255});
    DrawRectangleLines(20, H - 160, 120, 120, {60, 60, 60, 255});
    DrawTextEx(_font, "ART", {52, (float)(H - 108)}, 18, 1, dimText);

    // Track title and artist
    DrawTextEx(_font, _title.c_str(),  {160, (float)(H - 150)}, 36, 1, WHITE);
    DrawTextEx(_font, _artist.c_str(), {160, (float)(H - 106)}, 22, 1, dimText);

    DrawTextEx(_font, _playing ? "PLAYING" : "PAUSED",
               {160, (float)(H - 72)}, 16, 1, _playing ? barColor : dimText);

    // DrawRectangle(W - 168, H - 52, 160, 36, {40, 40, 40, 255});
    // DrawTextEx(_font, "BACK", {(float)(W - 140), (float)(H - 40)}, 16, 1, WHITE);

    std::string dbg = "BT: " + std::string(_connected ? "connected" : "disconnected");
    dbg += "  |  device: " + (_deviceName.empty() ? "none" : _deviceName);
    DrawTextEx(_font, dbg.c_str(), {8, (float)(H - 20)}, 12, 1, debugColor);

    DrawTextEx(_font, std::to_string(GetFPS()).c_str(), {50, 50}, 20, 1, RED);

    for (auto& widget : _widgets) {
        widget->draw();
    }
}

int MusicView::logic() {
    float dt = GetFrameTime();
    _fetchEvents();
    if (!_fftCfg) return 0;

    // Copy latest buffer safely
    std::array<float, kBufferSize> buf;
    {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        buf = _pcmBuffer;
    }

    // Apply Hann window to reduce spectral leakage
    for (int i = 0; i < kBufferSize; i++) {
        float window = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (kBufferSize - 1)));
        buf[i] *= window;
    }

    // Run FFT
    kiss_fftr(_fftCfg, buf.data(), _fftOut.data());

    // Map FFT bins to bar heights
    // Use logarithmic frequency spacing to match human hearing
    int bins = kBufferSize / 2 + 1;
    for (int bar = 0; bar < kBarCount; bar++) {
        float freqLow  = 20.0f  * powf(1000.0f, (float)bar / kBarCount);
        float freqHigh = 20.0f  * powf(1000.0f, (float)(bar + 1) / kBarCount);
        int binLow  = (int)(freqLow  * kBufferSize / kSampleRate);
        int binHigh = (int)(freqHigh * kBufferSize / kSampleRate);
        binLow  = std::max(1, std::min(binLow,  bins - 1));
        binHigh = std::max(1, std::min(binHigh, bins - 1));

        float magnitude = 0.0f;
        for (int b = binLow; b <= binHigh; b++) {
            float re = _fftOut[b].r;
            float im = _fftOut[b].i;
            magnitude = std::max(magnitude, sqrtf(re * re + im * im));
        }

        // Normalize and scale
        float target = std::min(1.0f, magnitude / 50.0f);
        _targetHeights[bar] = target;
    }

    // Smooth interpolation
    for (int i = 0; i < kBarCount; i++) {
        if (_targetHeights[i] > _barHeights[i]) {
            // Fast attack
            _barHeights[i] += (_targetHeights[i] - _barHeights[i]) * kSmoothingFactor * dt;
        } else {
            // Slow decay
            _barHeights[i] += (_targetHeights[i] - _barHeights[i]) * kDecayFactor * dt;
        }
    }
    return 0;
}

void MusicView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        switch (e->type) {
            case ViewEvent::Type::BLUETOOTH: {
                auto* bt = static_cast<BTEvent*>(e.get());
                switch (bt->btType) {
                    case BTEvent::BTType::DeviceConnected:
                        _connected  = true;
                        _deviceName = bt->deviceName;
                        break;
                    case BTEvent::BTType::DeviceDisconnected:
                        _connected  = false;
                        _title      = "No track";
                        _artist     = "No artist";
                        break;
                    case BTEvent::BTType::TrackChanged:
                        _title  = bt->title;
                        _artist = bt->artist;
                        break;
                    case BTEvent::BTType::PlaybackStateChanged:
                        _playing = bt->playing;
                        break;
                    default:
                        break;
                }
                break;
            }
            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType == InputEvent::InputType::TAP) {
                    for (auto& w : _widgets) {
                        if (w->handleEvent(*input)) break;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void MusicView::captureLoop() {
    std::array<float, kBufferSize> localBuf;
    int error;

    while (_capturing) {
        if (!_pulse) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (pa_simple_read(_pulse, localBuf.data(),
                           localBuf.size() * sizeof(float), &error) < 0) {
            std::cerr << "[VIS] Read error: " << pa_strerror(error) << "\n";
            continue;
        }

        std::lock_guard<std::mutex> lock(_bufferMutex);
        _pcmBuffer = localBuf;
    }
}