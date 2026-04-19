#include <views/MusicView.hpp>
#include <widgets/Button.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <cmath>
#include <iostream>

MusicView::MusicView() {}
MusicView::~MusicView() {}

void MusicView::start() {
    // Init PulseAudio monitor source
    pa_sample_spec spec;
    spec.format   = PA_SAMPLE_FLOAT32LE;
    spec.channels = 1;
    spec.rate     = kSampleRate;

    int error;
    _pulse = pa_simple_new(
        nullptr,
        "PiPOC",
        PA_STREAM_RECORD,
        "alsa_output.platform-bcm2835_audio.analog-stereo.monitor",
        "visualizer",
        &spec,
        nullptr,
        nullptr,
        &error
    );

    if (!_pulse)
        std::cerr << "[VIS] PulseAudio error: " << pa_strerror(error) << "\n";

    _fftCfg    = kiss_fftr_alloc(kBufferSize, 0, nullptr, nullptr);
    _capturing = true;
    _captureThread = std::jthread([this](std::stop_token st) { captureLoop(); });
}

void MusicView::close() {
    _capturing = false;
    _captureThread.request_stop();
    if (_captureThread.joinable()) _captureThread.join();

    if (_pulse) {
        pa_simple_free(_pulse);
        _pulse = nullptr;
    }
    if (_fftCfg) {
        kiss_fftr_free(_fftCfg);
        _fftCfg = nullptr;
    }

    _widgets.clear();
}

void MusicView::draw() {
    Color bg       = {10,  10,  10,  255};
    Color barColor = {39,  174, 96,  255};
    Color dimText  = {120, 120, 120, 255};

    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, bg);

    _topBar.draw();
    _bottomBar.draw();

    // Visualizer bars
    const int barW      = 16;
    const int barGap    = 4;
    const int totalBarW = kBarCount * (barW + barGap);
    const int barStartX = (DISPLAY_W - totalBarW) / 2;
    const int barBaseY  = (int)(DISPLAY_H * 0.62f);
    const int barMaxH   = (int)(DISPLAY_H * 0.55f);

    for (int i = 0; i < kBarCount; i++) {
        int bh = std::max(2, (int)(_barHeights[i] * barMaxH));
        int bx = barStartX + i * (barW + barGap);
        DrawRectangle(bx, barBaseY - bh, barW, bh, barColor);
    }

    // Track info — left aligned to bar grid, positioned below the bars
    int trackX  = barStartX;
    int trackY  = barBaseY + 16;
    int artistY = trackY + 52 + 4; // 52px title height + 4px gap
    DrawTextEx(Assets::catFont52, _title.c_str(),
               {(float)trackX, (float)trackY}, 52, 1, WHITE);
    DrawTextEx(Assets::catFont32, _artist.c_str(),
               {(float)trackX, (float)artistY}, 32, 1, dimText);

    for (auto& w : _widgets) w->draw();
}

int MusicView::logic() {
    float dt = GetFrameTime();
    _fetchEvents();
    if (!_fftCfg) return 0;

    std::array<float, kBufferSize> buf;
    {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        buf = _pcmBuffer;
    }

    // Hann window
    for (int i = 0; i < kBufferSize; i++) {
        float window = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (kBufferSize - 1)));
        buf[i] *= window;
    }

    kiss_fftr(_fftCfg, buf.data(), _fftOut.data());

    // Map FFT bins to bars with logarithmic frequency spacing
    int bins = kBufferSize / 2 + 1;
    for (int bar = 0; bar < kBarCount; bar++) {
        float freqLow  = 20.0f * powf(1000.0f, (float)bar / kBarCount);
        float freqHigh = 20.0f * powf(1000.0f, (float)(bar + 1) / kBarCount);
        int binLow  = std::max(1, std::min((int)(freqLow  * kBufferSize / kSampleRate), bins - 1));
        int binHigh = std::max(1, std::min((int)(freqHigh * kBufferSize / kSampleRate), bins - 1));

        float magnitude = 0.0f;
        for (int b = binLow; b <= binHigh; b++) {
            float re = _fftOut[b].r;
            float im = _fftOut[b].i;
            magnitude = std::max(magnitude, sqrtf(re * re + im * im));
        }
        _targetHeights[bar] = std::min(1.0f, magnitude / 50.0f);
    }

    // Smooth interpolation — fast attack, slow decay
    for (int i = 0; i < kBarCount; i++) {
        float factor = (_targetHeights[i] > _barHeights[i]) ? kSmoothingFactor : kDecayFactor;
        _barHeights[i] += (_targetHeights[i] - _barHeights[i]) * factor * dt;
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
                        ViewHandler::getInstance().setConnectedDevice(bt->deviceName);
                        break;
                    case BTEvent::BTType::DeviceDisconnected:
                        ViewHandler::getInstance().clearConnectedDevice();
                        _title  = "No track";
                        _artist = "No artist";
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
                    if (_bottomBar.handleEvent(*input)) break;
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