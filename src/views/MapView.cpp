#include <views/MapView.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <ViewHandler.hpp>
#include <raylib.h>
#include <cstdio>
#include <cmath>

static constexpr Color C_BG     = {10,  10,  10,  255};
static constexpr Color C_BORDER = {26,  26,  26,  255};
static constexpr Color C_VALUE  = {255, 255, 255, 255};
static constexpr Color C_UNIT   = {68,  68,  68,  255};
static constexpr Color C_WARN   = {180, 30,  30,  255};
static constexpr Color C_GOOD   = {0,   180, 100, 255};

static constexpr float FONT_24 = 24.0f;
static constexpr int   PAD     = 10;

MapView::MapView() {}
MapView::~MapView() {}
void MapView::start() {}
void MapView::close() {}

int MapView::logic() {
    _fetchEvents();
    return 0;
}

void MapView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        switch (e->type) {
            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType == InputEvent::InputType::TAP)
                    _bottomBar.handleEvent(*input);
                break;
            }
            default:
                break;
        }
    }
}

void MapView::draw() {
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, C_BG);

    _topBar.draw();
    _bottomBar.draw();

    const GPSData& gps = ViewHandler::getInstance().getGPSData();

    char buf[64];

    // Info bar sits just above the bottom bar
    int barH  = 48;
    int barY  = DISPLAY_H - BottomBar::HEIGHT - barH;
    DrawRectangle(0, barY, DISPLAY_W, barH, {10, 10, 10, 210});
    DrawRectangle(0, barY, DISPLAY_W, 1, C_BORDER);

    float ty = barY + (barH / 2 - (int)FONT_24 / 2);
    int   rx = DISPLAY_W - PAD;

    // Fix status + satellites — far right
    Color fixCol = gps.hasFix ? C_GOOD : C_WARN;
    const char* fixLabel = gps.hasFix ? "FIX" : "NO FIX";
    snprintf(buf, sizeof(buf), "%s  %dSAT", fixLabel, gps.satellites);
    int fixW = (int)MeasureTextEx(Assets::catFont24, buf, FONT_24, 1).x;
    rx -= fixW;
    DrawTextEx(Assets::catFont24, buf, {(float)rx, ty}, FONT_24, 1, fixCol);
    rx -= PAD;
    DrawRectangle(rx, barY + 8, 1, barH - 16, C_BORDER);
    rx -= PAD;

    // Heading
    if (gps.hasFix)
        snprintf(buf, sizeof(buf), "%.1f DEG", gps.heading);
    else
        snprintf(buf, sizeof(buf), "-- DEG");
    int hdgW = (int)MeasureTextEx(Assets::catFont24, buf, FONT_24, 1).x;
    rx -= hdgW;
    DrawTextEx(Assets::catFont24, buf, {(float)rx, ty}, FONT_24, 1, C_VALUE);
    rx -= PAD;
    DrawRectangle(rx, barY + 8, 1, barH - 16, C_BORDER);
    rx -= PAD;

    // Speed
    if (gps.hasFix)
        snprintf(buf, sizeof(buf), "%.1f KM/H", gps.speed);
    else
        snprintf(buf, sizeof(buf), "-- KM/H");
    int spdW = (int)MeasureTextEx(Assets::catFont24, buf, FONT_24, 1).x;
    rx -= spdW;
    DrawTextEx(Assets::catFont24, buf, {(float)rx, ty}, FONT_24, 1, C_VALUE);
    rx -= PAD;
    DrawRectangle(rx, barY + 8, 1, barH - 16, C_BORDER);
    rx -= PAD;

    // Lat / Lon — left side
    if (gps.hasFix)
        snprintf(buf, sizeof(buf), "%.6f %s  %.6f %s",
                 std::abs(gps.lat), gps.lat >= 0 ? "N" : "S",
                 std::abs(gps.lon), gps.lon >= 0 ? "E" : "W");
    else
        snprintf(buf, sizeof(buf), "-- N  -- E");
    DrawTextEx(Assets::catFont24, buf, {(float)PAD, ty}, FONT_24, 1, C_VALUE);
}