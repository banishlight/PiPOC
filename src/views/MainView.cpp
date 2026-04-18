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

static constexpr int   TOPBAR_H    = 36;
static constexpr int   BOTBAR_H    = 40;
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

static constexpr int GEAR_X    = DISPLAY_W - 40;
static constexpr int GEAR_Y    = DISPLAY_H - BOTBAR_H + 8;
static constexpr int GEAR_SIZE = 24;

MainView::MainView() {
    _initButtons();
}

MainView::~MainView() {}

void MainView::start() {}
void MainView::close() {}

void MainView::_initButtons() {
    // Nav buttons — ECU, AUDIO, MAPS
    struct NavDef { const char* label; const char* sublabel; std::function<void()> action; };
    NavDef defs[BTN_COUNT] = {
        { "ECU",   "LIVE DATA",    [](){ ViewHandler::getInstance().switchView(std::make_unique<OBDView>()); } },
        { "AUDIO", "MEDIA PLAYER", [](){ ViewHandler::getInstance().switchView(std::make_unique<MusicView>()); } },
        { "MAPS",  "NAVIGATION",   [](){ ViewHandler::getInstance().switchView(std::make_unique<MapView>()); } },
    };

    for (int i = 0; i < BTN_COUNT; i++) {
        int bx = BTN_GRID_X + i * (BTN_W + BTN_GAP);
        auto btn = std::make_unique<Button>(bx, BTN_GRID_Y, BTN_W, BTN_H, defs[i].label);
        btn->setSublabel(defs[i].sublabel);
        btn->setOnClick(defs[i].action);
        btn->setColors(BG2, BG2, RED_ACCENT, TEXT_MID);
        btn->setAnimationStyle(Button::AnimationStyle::SweepFill, 0.15f);
        _navButtons.push_back(std::move(btn));
    }

    // Settings/gear button — no animation, just fires immediately
    _settingsButton = std::make_unique<Button>(GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE, "");
    _settingsButton->setOnClick([](){ ViewHandler::getInstance().switchView(std::make_unique<SettingsView>()); });
    _settingsButton->setAnimationStyle(Button::AnimationStyle::None);
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

                // Settings button
                if (_settingsButton->handleEvent(*input)) return;

                // Nav buttons
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

    // Top bar
    DrawRectangle(0, 0, DISPLAY_W, TOPBAR_H, BG2);
    DrawRectangle(0, TOPBAR_H - 1, DISPLAY_W, 1, BORDER);
    DrawTextEx(Assets::catFont16, "MAZDA RX-8  13B-MSP", {16, 10}, 16, 3, TEXT_DIM);

    // Clock
    char clockBuf[16];
    time_t now = time(nullptr);
    strftime(clockBuf, sizeof(clockBuf), "%H:%M:%S", localtime(&now));
    int cw = (int)MeasureTextEx(Assets::catFont16, clockBuf, 16, 1).x;
    DrawTextEx(Assets::catFont16, clockBuf, {(float)(DISPLAY_W - cw - 16), 10}, 16, 1, TEXT_DIM);
    DrawRectangle(DISPLAY_W - cw - 32, 8, 1, 20, BORDER);

    // BT status
    const char* btLabel = ViewHandler::getInstance().isDeviceConnected() ? "BT  CONNECTED" : "BT  DISCONNECTED";
    DrawTextEx(Assets::catFont16, btLabel, {(float)(DISPLAY_W - cw - 148), 10}, 16, 2, TEXT_DIM);
    DrawRectangle(DISPLAY_W - cw - 164, 8, 1, 20, BORDER);
    DrawTextEx(Assets::catFont16, "ECU  ONLINE", {(float)(DISPLAY_W - cw - 268), 10}, 16, 2, TEXT_DIM);

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

    // Bottom bar
    DrawRectangle(0, DISPLAY_H - BOTBAR_H, DISPLAY_W, BOTBAR_H, BG2);
    DrawRectangle(0, DISPLAY_H - BOTBAR_H, DISPLAY_W, 1, BORDER);

    DrawCircle(22, DISPLAY_H - BOTBAR_H + 20, 3, RED_ACCENT);
    DrawTextEx(Assets::catFont16,
               ViewHandler::getInstance().getConnectedDevice().c_str(),
               {34, (float)(DISPLAY_H - BOTBAR_H + 11)}, 16, 2, TEXT_DIM);

    const char* ver = "v0.1.0  PIPOC";
    int vw = (int)MeasureTextEx(Assets::catFont16, ver, 16, 2).x;
    DrawTextEx(Assets::catFont16, ver,
               {(float)(DISPLAY_W - vw - GEAR_SIZE - 24), (float)(DISPLAY_H - BOTBAR_H + 11)},
               16, 2, {34, 34, 34, 255});

    // Gear icon — drawn on top of the invisible settings button
    DrawTexture(Assets::gearIcon, GEAR_X, GEAR_Y, TEXT_DIM);
}