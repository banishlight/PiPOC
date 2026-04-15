#include <views/MusicView.hpp>
#include <cmath>
#include <iostream>

MusicView::MusicView() {}

MusicView::~MusicView() {}

void MusicView::start() {
    _font = LoadFontEx(_fontPath.c_str(), 64, nullptr, 0);
}

void MusicView::close() {
    
}

void MusicView::draw() {
    const int W = 1024;
    const int H = 600;

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

    for (int i = 0; i < barCount; i++) {
        // Placeholder sine wave shape so it looks like the reference
        float norm = (float)i / barCount;
        float height = barMaxH * (
            0.8f * expf(-6.0f * norm) +
            0.3f * sinf(norm * 6.28f) +
            0.1f
        );
        int bh = (int)height;
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

    DrawRectangle(W - 168, H - 52, 160, 36, {40, 40, 40, 255});
    DrawTextEx(_font, "BACK", {(float)(W - 140), (float)(H - 40)}, 16, 1, WHITE);

    std::string dbg = "BT: " + std::string(_connected ? "connected" : "disconnected");
    dbg += "  |  device: " + (_deviceName.empty() ? "none" : _deviceName);
    DrawTextEx(_font, dbg.c_str(), {8, (float)(H - 20)}, 12, 1, debugColor);
}

int MusicView::logic() {
    _fetchEvents();

    return 0;
}

void MusicView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        switch (e->type) {
            case ViewEvent::Type::BLUETOOTH: {
                auto* bt = static_cast<BTEvent*>(e.get());
                // Debugging
                std::cout << "[MusicView] BTEvent received, btType: " << (int)bt->btType << "\n";
                std::cout << "[MusicView] Title: " << bt->title << "\n";
                std::cout << "[MusicView] Artist: " << bt->artist << "\n";
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