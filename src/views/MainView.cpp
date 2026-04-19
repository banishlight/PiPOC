#include <views/MainView.hpp>
#include <views/OBDView.hpp>
#include <views/MusicView.hpp>
#include <views/SettingsView.hpp>
#include <views/MapView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>
#include <cstring>
#include <ctime>

static constexpr int   TOPBAR_H    = TopBar::HEIGHT;
static constexpr int   BOTBAR_H    = BottomBar::HEIGHT;
static constexpr Color BG          = {10,  10,  10,  255};
static constexpr Color BG2         = {15,  15,  15,  255};
static constexpr Color BORDER      = {26,  26,  26,  255};
static constexpr Color RED_ACCENT  = {255, 40,  0,   255};
static constexpr Color TEXT_DIM    = {80,  80,  80,  255};
static constexpr Color TEXT_MID    = {140, 140, 140, 255};
static constexpr Color TEXT_BRIGHT = {220, 220, 220, 255};

static constexpr int BTN_COUNT  = 3;
static constexpr int BTN_W      = 200;
static constexpr int BTN_H      = 90;
static constexpr int BTN_GAP    = 2;
static constexpr int BTN_GRID_W = BTN_COUNT * BTN_W + (BTN_COUNT - 1) * BTN_GAP;
static constexpr int BTN_GRID_X = (DISPLAY_W - BTN_GRID_W) / 2;
static constexpr int BTN_GRID_Y = (DISPLAY_H - BTN_H) / 2 + 40;

MainView::MainView() {
    _initButtons();
}

MainView::~MainView() {}
void MainView::start() {}
void MainView::close() {}

void MainView::_initButtons() {
    struct NavDef { const char* label; const char* sublabel; std::function<void()> action; };
    NavDef defs[BTN_COUNT] = {
        { "ECU",   "LIVE DATA",    [](){ ViewHandler::getInstance().switchView(std::make_unique<OBDView>()); } },
        { "AUDIO", "MEDIA PLAYER", [](){ ViewHandler::getInstance().switchView(std::make_unique<MusicView>()); } },
        { "MAPS",  "NAVIGATION",   [](){ ViewHandler::getInstance().switchView(std::make_unique<MapView>()); } },
    };

    for (int i = 0; i < BTN_COUNT; i++) {
        int bx  = BTN_GRID_X + i * (BTN_W + BTN_GAP);
        auto btn = std::make_unique<Button>(bx, BTN_GRID_Y, BTN_W, BTN_H, defs[i].label);
        btn->setSublabel(defs[i].sublabel);
        btn->setOnClick(defs[i].action);
        btn->setColors(BG2, TEXT_MID);
        _navButtons.push_back(std::move(btn));
    }
}

int MainView::logic() {
    _fetchEvents();
    return 0;
}

void MainView::_fetchEvents() {
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

                if (_bottomBar.handleEvent(*input)) break;

                for (auto& btn : _navButtons) {
                    if (btn->handleEvent(*input)) break;
                }
                break;
            }

            default:
                break;
        }
    }
}

void MainView::draw() {
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, BG);

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

    _topBar.draw();
    _bottomBar.draw();

    // Title block
    int titleY = TOPBAR_H + 60;
    const char* title = "RX-8";
    int tw = (int)MeasureTextEx(Assets::catFont52, title, 52, 4).x;
    DrawTextEx(Assets::catFont52, title, {(float)((DISPLAY_W - tw) / 2), (float)titleY}, 52, 4, TEXT_BRIGHT);
    DrawRectangle((DISPLAY_W - 120) / 2, titleY + 62, 120, 2, RED_ACCENT);
    const char* sub = "SELECT SYSTEM";
    int sw = (int)MeasureTextEx(Assets::catFont16, sub, 16, 8).x;
    DrawTextEx(Assets::catFont16, sub, {(float)((DISPLAY_W - sw) / 2), (float)(titleY + 72)}, 16, 8, TEXT_DIM);

    // Nav buttons
    for (auto& btn : _navButtons)
        btn->draw();
}