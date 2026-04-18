#include <views/SettingsView.hpp>
#include <views/MainView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>

static constexpr Color BG         = {10,  10,  10,  255};
static constexpr Color BG2        = {15,  15,  15,  255};
static constexpr Color BORDER     = {26,  26,  26,  255};
static constexpr Color RED_ACCENT = {255, 40,  0,   255};
static constexpr Color TEXT_DIM   = {80,  80,  80,  255};
static constexpr Color TEXT_BRIGHT= {220, 220, 220, 255};

SettingsView::SettingsView() {}
SettingsView::~SettingsView() {}

void SettingsView::start() {
    // Fullscreen toggle
    auto fsBtn = std::make_unique<Button>(412, 260, 200, 52, "FULLSCREEN");
    fsBtn->setOnClick([]() { ToggleFullscreen(); });
    _widgets.push_back(std::move(fsBtn));

    // Back
    auto backBtn = std::make_unique<Button>(412, 320, 200, 52, "BACK");
    backBtn->setOnClick([](){ ViewHandler::getInstance().switchView(std::make_unique<MainView>()); });
    _widgets.push_back(std::move(backBtn));
}

void SettingsView::close() {
    _widgets.clear();
}

int SettingsView::logic() {
    _fetchEvents();
    return 0;
}

void SettingsView::_fetchEvents() {
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
                        break;
                    default:
                        break;
                }
                break;
            }
            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType != InputEvent::InputType::TAP) break;
                for (auto& w : _widgets) {
                    if (w->handleEvent(*input)) break;
                }
                break;
            }
            default:
                break;
        }
    }
}

void SettingsView::draw() {
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, BG);

    // Top bar
    DrawRectangle(0, 0, DISPLAY_W, 36, BG2);
    DrawRectangle(0, 35, DISPLAY_W, 1, BORDER);
    DrawTextEx(Assets::catFont16, "SETTINGS", {16, 11}, 16, 3, TEXT_DIM);

    // Corner brackets
    int bSize = 20, bThick = 2;
    DrawRectangle(8, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bSize, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bThick, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(8, DISPLAY_H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, DISPLAY_H - 8 - bSize, bThick, bSize, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bSize, DISPLAY_H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bThick, DISPLAY_H - 8 - bSize, bThick, bSize, RED_ACCENT);

    // Section label
    DrawTextEx(Assets::catFont16, "DISPLAY", {16, 52}, 16, 3, RED_ACCENT);
    DrawRectangle(16, 72, DISPLAY_W - 32, 1, BORDER);

    // Widgets
    for (auto& w : _widgets) w->draw();

    // Bottom bar
    DrawRectangle(0, DISPLAY_H - 40, DISPLAY_W, 40, BG2);
    DrawRectangle(0, DISPLAY_H - 40, DISPLAY_W, 1, BORDER);
}