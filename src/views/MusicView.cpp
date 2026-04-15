#include <views/MusicView.hpp>

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

    DrawRectangle(0, 0, W, H, {10, 10, 10, 255});

    // Connection status
    Color statusColor = _connected ? GREEN : RED;
    DrawTextEx(_font, _connected ? "Connected" : "Disconnected",
               {20, 20}, 20, 1, statusColor);
    DrawTextEx(_font, _deviceName.c_str(), {20, 46}, 16, 1, GRAY);

    // Track info
    DrawTextEx(_font, _title.c_str(),  {20, 300}, 48, 1, WHITE);
    DrawTextEx(_font, _artist.c_str(), {20, 360}, 28, 1, GRAY);

    // Back button
    DrawRectangle(8, H - 52, 160, 36, {60, 60, 60, 255});
    DrawTextEx(_font, "BACK", {28, (float)(H - 40)}, 16, 1, WHITE);
}

int MusicView::logic() {


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