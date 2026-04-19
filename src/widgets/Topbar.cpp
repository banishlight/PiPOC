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

static constexpr float FONT = 24.0f;
static constexpr int   PAD  = 10;
static constexpr int   SEP_H = TopBar::HEIGHT - 16; // separator line height

TopBar::TopBar() {}

void TopBar::draw() {
    if (!_visible) return;

    DrawRectangle(0, 0, DISPLAY_W, TopBar::HEIGHT, C_BG);
    DrawRectangle(0, TopBar::HEIGHT - 1, DISPLAY_W, 1, C_BORDER);

    float ry = (float)(TopBar::HEIGHT / 2 - (int)FONT / 2);
    int   sep = (TopBar::HEIGHT - SEP_H) / 2;

    // -- Left: car model --
    DrawTextEx(Assets::catFont24, CAR_NAME,
               {(float)PAD, ry}, FONT, 1, C_DIM);

    // -- Right side built right to left --
    int rx = DISPLAY_W - PAD;

    // Clock
    char clockBuf[16];
    time_t now = time(nullptr);
    strftime(clockBuf, sizeof(clockBuf), "%H:%M:%S", localtime(&now));
    rx -= (int)MeasureTextEx(Assets::catFont24, clockBuf, FONT, 1).x;
    DrawTextEx(Assets::catFont24, clockBuf, {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, sep, 1, SEP_H, C_BORDER);
    rx -= PAD;

    // CPU temp
    char tempBuf[16];
    snprintf(tempBuf, sizeof(tempBuf), "%.0fC", ViewHandler::getInstance().getCPUTemp());
    int cpuLabel = (int)MeasureTextEx(Assets::catFont24, "CPU ", FONT, 1).x;
    int cpuTemp  = (int)MeasureTextEx(Assets::catFont24, tempBuf, FONT, 1).x;
    rx -= cpuTemp;
    DrawTextEx(Assets::catFont24, tempBuf, {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= cpuLabel;
    DrawTextEx(Assets::catFont24, "CPU ", {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, sep, 1, SEP_H, C_BORDER);
    rx -= PAD;

    // CPU load
    char loadBuf[16];
    snprintf(loadBuf, sizeof(loadBuf), "%.0f%%", ViewHandler::getInstance().getCPULoad());
    int loadLabel = (int)MeasureTextEx(Assets::catFont24, "LOAD ", FONT, 1).x;
    int loadVal   = (int)MeasureTextEx(Assets::catFont24, loadBuf, FONT, 1).x;
    rx -= loadVal;
    DrawTextEx(Assets::catFont24, loadBuf, {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= loadLabel;
    DrawTextEx(Assets::catFont24, "LOAD ", {(float)rx, ry}, FONT, 1, C_DIM);
    rx -= PAD;
    DrawRectangle(rx, sep, 1, SEP_H, C_BORDER);
    rx -= PAD;

    // WiFi status
    bool wifi = ViewHandler::getInstance().isWifiConnected();
    const char* wifiLabel = "WIFI";
    Color wifiCol = wifi ? C_GOOD : C_WARN;
    rx -= (int)MeasureTextEx(Assets::catFont24, wifiLabel, FONT, 1).x;
    DrawTextEx(Assets::catFont24, wifiLabel, {(float)rx, ry}, FONT, 1, wifiCol);
    rx -= PAD;
    DrawRectangle(rx, sep, 1, SEP_H, C_BORDER);
    rx -= PAD;

    // ECU status
    bool ecu = ViewHandler::getInstance().isECUOnline();
    const char* ecuLabel = ecu ? "ECU  ONLINE" : "ECU  OFFLINE";
    Color ecuCol = ecu ? C_GOOD : C_WARN;
    rx -= (int)MeasureTextEx(Assets::catFont24, ecuLabel, FONT, 1).x;
    DrawTextEx(Assets::catFont24, ecuLabel, {(float)rx, ry}, FONT, 1, ecuCol);
}