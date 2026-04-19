#include <widgets/Bottombar.hpp>
#include <views/MainView.hpp>
#include <views/SettingsView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>

static constexpr Color CB_BG       = {15,  15,  15,  255};
static constexpr Color CB_BORDER   = {26,  26,  26,  255};
static constexpr Color CB_DIM      = {85,  85,  85,  255};
static constexpr Color CB_RED      = {255, 40,  0,   255};
static constexpr Color CB_MENU_BG  = {0,   60,  120, 255};
static constexpr Color CB_MENU_TEXT= {170, 200, 240, 255};
static constexpr Color CB_VER      = {34,  34,  34,  255};

static constexpr float FONT = 16.0f;
static constexpr int   PAD  = 8;

static constexpr int BTN_W = 120;
static constexpr int BTN_H = BottomBar::HEIGHT - 8;
static constexpr int BTN_Y = DISPLAY_H - BottomBar::HEIGHT + 4;

BottomBar::BottomBar(bool showMainMenu) : _showMainMenu(showMainMenu) {
    // Settings button — uses gear icon
    int settingsX = showMainMenu
        ? DISPLAY_W - BTN_W - PAD - Assets::gearIcon.width - PAD - PAD
        : DISPLAY_W - Assets::gearIcon.width - PAD;
    _settingsButton = std::make_unique<Button>(
        DISPLAY_W - Assets::gearIcon.width - PAD,
        DISPLAY_H - BottomBar::HEIGHT + (BottomBar::HEIGHT - Assets::gearIcon.height) / 2,
        Assets::gearIcon.width,
        Assets::gearIcon.height,
        ""
    );
    _settingsButton->setImage(Assets::gearIcon);
    _settingsButton->setOnClick([](){ 
        ViewHandler::getInstance().switchView(std::make_unique<SettingsView>()); 
    });

    // Optional main menu button
    if (showMainMenu) {
        _mainMenuButton = std::make_unique<Button>(
            DISPLAY_W - BTN_W - Assets::gearIcon.width - PAD * 2,
            BTN_Y,
            BTN_W,
            BTN_H,
            "MAIN MENU"
        );
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

    // BT dot + device name
    DrawCircle(PAD + 4, y + BottomBar::HEIGHT / 2, 3, CB_RED);
    DrawTextEx(Assets::catFont16,
               ViewHandler::getInstance().getConnectedDevice().c_str(),
               {(float)(PAD + 14), (float)(y + BottomBar::HEIGHT / 2 - FONT / 2)},
               FONT, 1, CB_DIM);

    // Software version — centred
    char verBuf[32];
    snprintf(verBuf, sizeof(verBuf), "v%s  PIPOC", VERSION);
    int vw = (int)MeasureTextEx(Assets::catFont16, verBuf, FONT, 1).x;
    DrawTextEx(Assets::catFont16, verBuf,
               {(float)((DISPLAY_W - vw) / 2), (float)(y + BottomBar::HEIGHT / 2 - FONT / 2)},
               FONT, 1, CB_VER);

    // Buttons
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