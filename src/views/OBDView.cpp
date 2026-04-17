#include <views/OBDView.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <raylib.h>

OBDView::OBDView() {}

OBDView::~OBDView() {}

void OBDView::start() {
    
}

void OBDView::close() {

}

void OBDView::draw() {
    // -- Placeholder data (replace with real OBD values) --
    float rpm         = 3500.0f;
    float coolant     = 87.0f;
    float oilTemp     = 102.0f;
    float throttle    = 45.0f;
    float speed       = 72.0f;
    float intakeTemp  = 28.0f;
    float maf         = 12.4f;
    float voltage     = 14.2f;

    // -- Colors --
    Color bg          = {10,  10,  10,  255};
    Color cellBg      = {20,  20,  20,  255};
    Color cellBorder  = {40,  40,  40,  255};
    Color labelColor  = {0,   180, 100, 255};
    Color valueColor  = {255, 255, 255, 255};
    Color primaryColor= {220, 220, 0,   255};
    Color warnColor   = {180, 30,  30,  255};
    Color barColor    = {30,  30,  30,  255};

    auto drawCell = [&](int x, int y, int w, int h,
                        const char* label, const char* value,
                        Color bg2, Color lc, Color vc, int valueFontSize) {
        DrawRectangle(x, y, w, h, bg2);
        DrawRectangleLines(x, y, w, h, cellBorder);
        DrawTextEx(Assets::mainFont, label, {(float)(x + 8), (float)(y + 8)}, 14, 1, lc);
        int vw = (int)MeasureTextEx(Assets::mainFont, value, (float)valueFontSize, 1).x;
        DrawTextEx(Assets::mainFont, value, {(float)(x + w - vw - 12), (float)(y + h - valueFontSize - 12)}, (float)valueFontSize, 1, vc);
    };

    // Background
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, bg);

    // Top bar
    DrawRectangle(0, 0, DISPLAY_W, 36, barColor);
    DrawTextEx(Assets::mainFont, "OBD LIVE DATA", {(float)(DISPLAY_W/2 - 80), 10}, 16, 1, GRAY);

    // Layout constants
    int topY    = 36;
    int botY    = DISPLAY_H - 52;
    int gridH   = botY - topY;

    int leftW   = 280;
    int centerW = 200;
    int rightW  = DISPLAY_W - leftW - centerW;
    int centerX = leftW;
    int rightX  = leftW + centerW;

    int cellH   = gridH / 4;

    // -- Left column --
    char buf[32];

    snprintf(buf, sizeof(buf), "%.0f", rpm);
    drawCell(0, topY + cellH*0, leftW, cellH,
             "RPM", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.1f", speed);
    drawCell(0, topY + cellH*1, leftW, cellH,
             "SPEED (km/h)", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.1f", throttle);
    drawCell(0, topY + cellH*2, leftW, cellH,
             "THROTTLE (%)", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.1f", voltage);
    drawCell(0, topY + cellH*3, leftW, cellH,
             "VOLTAGE (V)", buf, cellBg, labelColor, valueColor, 36);

    // -- Center column --
    snprintf(buf, sizeof(buf), "%.1f", maf);
    drawCell(centerX, topY, centerW, cellH,
             "MAF (g/s)", buf, cellBg, labelColor, primaryColor, 28);

    int bigH = cellH * 2;
    DrawRectangle(centerX, topY + cellH, centerW, bigH, {20, 20, 0, 255});
    DrawRectangleLines(centerX, topY + cellH, centerW, bigH, cellBorder);
    snprintf(buf, sizeof(buf), "%.0f", coolant);
    int bigFontSize = 80;
    int bw = (int)MeasureTextEx(Assets::mainFont, buf, (float)bigFontSize, 1).x;
    DrawTextEx(Assets::mainFont, buf, {(float)(centerX + (centerW - bw) / 2), (float)(topY + cellH + (bigH - bigFontSize) / 2)}, (float)bigFontSize, 1, primaryColor);
    DrawTextEx(Assets::mainFont, "COOLANT", {(float)(centerX + 8), (float)(topY + cellH + 8)}, 13, 1, labelColor);

    snprintf(buf, sizeof(buf), "%.0f", oilTemp);
    Color oilBg = (oilTemp > 120.0f) ? warnColor : cellBg;
    drawCell(centerX, topY + cellH*3, centerW, cellH,
             "OIL (C)", buf, oilBg, labelColor, valueColor, 28);

    // -- Right column --
    int rCellW = rightW / 2;
    int rCellH = gridH / 2;

    snprintf(buf, sizeof(buf), "%.1f", intakeTemp);
    drawCell(rightX,          topY,          rCellW, rCellH,
             "INTAKE TEMP (C)", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.1f", oilTemp);
    drawCell(rightX + rCellW, topY,          rCellW, rCellH,
             "OIL TEMP (C)", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.2f", maf);
    drawCell(rightX,          topY + rCellH, rCellW, rCellH,
             "MAF (g/s)", buf, cellBg, labelColor, valueColor, 36);

    snprintf(buf, sizeof(buf), "%.1f%%", throttle);
    drawCell(rightX + rCellW, topY + rCellH, rCellW, rCellH,
             "THROTTLE", buf, cellBg, labelColor, valueColor, 36);

    // Bottom bar
    DrawRectangle(0, botY, DISPLAY_W, DISPLAY_H - botY, barColor);

    DrawRectangle(8, botY + 8, 160, 36, warnColor);
    DrawTextEx(Assets::mainFont, "SETTINGS", {28, (float)(botY + 18)}, 16, 1, WHITE);

    DrawRectangle(DISPLAY_W - 168, botY + 8, 160, 36, {0, 60, 120, 255});
    DrawTextEx(Assets::mainFont, "MAIN MENU", {(float)(DISPLAY_W - 152), (float)(botY + 18)}, 16, 1, WHITE);
}

int OBDView::logic() {
    return 0;
}

void OBDView::fetchEvents() {}