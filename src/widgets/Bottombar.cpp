#include <widgets/Bottombar.hpp>
#include <views/MainView.hpp>
#include <views/SettingsView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>
#include <cstdio>

static constexpr Color CB_BG        = {15,  15,  15,  255};
static constexpr Color CB_BORDER    = {26,  26,  26,  255};
static constexpr Color CB_DIM       = {85,  85,  85,  255};
static constexpr Color CB_RED       = {255, 40,  0,   255};
static constexpr Color CB_MENU_BG   = {0,   60,  120, 255};
static constexpr Color CB_MENU_TEXT = {170, 200, 240, 255};
static constexpr Color CB_VER       = {34,  34,  34,  255};

static constexpr float FONT   = 24.0f;
static constexpr int   PAD    = 10;
static constexpr int   BTN_W  = 140;
static constexpr int   BTN_H  = BottomBar::HEIGHT - 10;
static constexpr int   BTN_Y  = DISPLAY_H - BottomBar::HEIGHT + 5;

BottomBar::BottomBar(bool showMainMenu) : _showMainMenu(showMainMenu) {
    int gearSize = Assets::gearIcon.width;
    int gearY    = DISPLAY_H - BottomBar::HEIGHT + (BottomBar::HEIGHT - Assets::gearIcon.height) / 2;
    int gearX    = DISPLAY_W - gearSize - PAD;

    _settingsButton = std::make_unique<Button>(gearX, gearY, gearSize, gearSize, "");
    _settingsButton->setImage(Assets::gearIcon);
    _settingsButton->setOnClick([](){ 
        ViewHandler::getInstance().switchView(std::make_unique<SettingsView>()); 
    });

    if (showMainMenu) {
        int btnX = gearX - BTN_W - PAD;
        _mainMenuButton = std::make_unique<Button>(btnX, BTN_Y, BTN_W, BTN_H, "MAIN MENU");
        _mainMenuButton->setColors(CB_MENU_BG, CB_MENU_TEXT);
        _mainMenuButton->setOnClick([](){ 
            ViewHandler::getInstance().switchView(std::make_unique<MainView>()); 
        });
    }
}

void BottomBar::draw() {
    if (!_visible) return;

    int y = DISPLAY_H - BottomBar::HEIGHT;
    DrawRectangle(0, y, DISPLAY_W, BottomBar::HEIGHT, CB_BG);
    DrawRectangle(0, y, DISPLAY_W, 1, CB_BORDER);

    float ty = (float)(y + BottomBar::HEIGHT / 2 - (int)FONT / 2);

    // BT dot + device name
    DrawCircle(PAD + 5, y + BottomBar::HEIGHT / 2, 4, CB_RED);
    DrawTextEx(Assets::catFont24,
               ViewHandler::getInstance().getConnectedDevice().c_str(),
               {(float)(PAD + 16), ty}, FONT, 1, CB_DIM);

    // Software version — centred
    char verBuf[32];
    snprintf(verBuf, sizeof(verBuf), "v%s  %s", VERSION, SOFTWARE_NAME);
    int vw = (int)MeasureTextEx(Assets::catFont24, verBuf, FONT, 1).x;
    DrawTextEx(Assets::catFont24, verBuf,
               {(float)((DISPLAY_W - vw) / 2), ty}, FONT, 1, CB_VER);

    if (_showMainMenu && _mainMenuButton)
        _mainMenuButton->draw();

    _settingsButton->draw();
}

bool BottomBar::handleEvent(const InputEvent& event) {
    if (!_visible) return false;

    if (_showMainMenu && _mainMenuButton) {
        if (_mainMenuButton->handleEvent(event)) return true;
    }

    return _settingsButton->handleEvent(event);
}