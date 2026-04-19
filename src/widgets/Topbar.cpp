#include <widgets/Topbar.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>
#include <cstdio>
#include <ctime>

static constexpr Color C_BG     = {15,  15,  15,  255};
static constexpr Color C_BORDER = {26,  26,  26,  255};
static constexpr Color C_DIM    = {85,  85,  85,  255};
static constexpr Color C_GOOD   = {0,   180, 100, 255};
static constexpr Color C_WARN   = {180, 30,  30,  255};

static constexpr float FONT = 16.0f;
static constexpr int   PAD  = 8;

TopBar::TopBar() {}

void TopBar::draw() {
    if (!_visible) return;

    DrawRectangle(0, 0, DISPLAY_W, TopBar::HEIGHT, C_BG);
    DrawRectangle(0, TopBar::HEIGHT - 1, DISPLAY_W, 1, C_BORDER);

    // -- Left side: car model --
    DrawTextEx(Assets::catFont16, CAR_NAME,
               {(float)PAD, (float)(TopBar::HEIGHT / 2 - FONT / 2)},
               FONT, 1, C_DIM);

    // -- Right side: built right to left --
    // Clock
    char clockBuf[16];
    time_t now = time(nullptr);
    strftime(clockBuf, sizeof(clockBuf), "%H:%M:%S", localtime(&now));
    int clockW = (int)MeasureTextEx(Assets::catFont16, clockBuf, FONT, 1).x;
    float ry = (float)(TopBar::HEIGHT / 2 - FONT / 2);
    int rx = DISPLAY_W - PAD;

    rx -= clockW;
    DrawTextEx(Assets::catFont16, clockBuf, {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, 6, 1, 20, C_BORDER);
    rx -= PAD;

    // CPU temp
    char tempBuf[16];
    snprintf(tempBuf, sizeof(tempBuf), "%.0f C", ViewHandler::getInstance().getCPUTemp());
    int tempW = (int)MeasureTextEx(Assets::catFont16, tempBuf, FONT, 1).x;
    rx -= tempW;
    DrawTextEx(Assets::catFont16, tempBuf, {(float)rx, ry}, FONT, 1, C_DIM);

    // CPU temp label
    int cpuTempLabelW = (int)MeasureTextEx(Assets::catFont16, "CPU ", FONT, 1).x;
    rx -= cpuTempLabelW;
    DrawTextEx(Assets::catFont16, "CPU ", {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, 6, 1, 20, C_BORDER);
    rx -= PAD;

    // CPU load
    char loadBuf[16];
    snprintf(loadBuf, sizeof(loadBuf), "%.0f%%", ViewHandler::getInstance().getCPULoad());
    int loadW = (int)MeasureTextEx(Assets::catFont16, loadBuf, FONT, 1).x;
    rx -= loadW;
    DrawTextEx(Assets::catFont16, loadBuf, {(float)rx, ry}, FONT, 1, C_DIM);

    int loadLabelW = (int)MeasureTextEx(Assets::catFont16, "LOAD ", FONT, 1).x;
    rx -= loadLabelW;
    DrawTextEx(Assets::catFont16, "LOAD ", {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, 6, 1, 20, C_BORDER);
    rx -= PAD;

    // WiFi status
    bool wifi = ViewHandler::getInstance().isWifiConnected();
    const char* wifiLabel = wifi ? "WIFI" : "WIFI";
    Color wifiCol = wifi ? C_GOOD : C_WARN;
    int wifiW = (int)MeasureTextEx(Assets::catFont16, wifiLabel, FONT, 1).x;
    rx -= wifiW;
    DrawTextEx(Assets::catFont16, wifiLabel, {(float)rx, ry}, FONT, 1, wifiCol);
    rx -= PAD;
    DrawRectangle(rx, 6, 1, 20, C_BORDER);
    rx -= PAD;

    // ECU status
    bool ecu = ViewHandler::getInstance().isECUOnline();
    const char* ecuLabel = ecu ? "ECU  ONLINE" : "ECU  OFFLINE";
    Color ecuCol = ecu ? C_GOOD : C_WARN;
    int ecuW = (int)MeasureTextEx(Assets::catFont16, ecuLabel, FONT, 1).x;
    rx -= ecuW;
    DrawTextEx(Assets::catFont16, ecuLabel, {(float)rx, ry}, FONT, 1, ecuCol);
}