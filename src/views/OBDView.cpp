#include <views/OBDView.hpp>
#include <raylib.h>

OBDView::OBDView() {}

OBDView::~OBDView() {}

void OBDView::start() {
    // _font = LoadFont("cnr.otf");
}

void OBDView::close() {
    // UnloadFont(_font);
}

void OBDView::draw() {
    const int W = 1024;
    const int H = 600;

    // -- Placeholder data (replace with real OBD values later) --
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
    Color labelColor  = {0,   180, 100, 255};  // green labels
    Color valueColor  = {255, 255, 255, 255};  // white values
    Color primaryColor= {220, 220, 0,   255};  // yellow primary
    Color warnColor   = {180, 30,  30,  255};  // red warning cells
    Color barColor    = {30,  30,  30,  255};  // top/bottom bar

    // ----------------------------------------------------------------
    // Background
    // ----------------------------------------------------------------
    DrawRectangle(0, 0, W, H, bg);

    // ----------------------------------------------------------------
    // Top bar
    // ----------------------------------------------------------------
    DrawRectangle(0, 0, W, 36, barColor);
    DrawText("OBD LIVE DATA", W/2 - 80, 10, 16, GRAY);

    // ----------------------------------------------------------------
    // Layout constants
    // ----------------------------------------------------------------
    int topY     = 36;
    int botY     = H - 52;
    int gridH    = botY - topY;   // 512px

    // Three columns: left | center | right
    int leftW    = 280;
    int centerW  = 200;
    int rightW   = W - leftW - centerW;  // 544px
    int centerX  = leftW;
    int rightX   = leftW + centerW;

    // Left column — 4 cells stacked
    int cellH    = gridH / 4;  // 128px each

    auto drawCell = [&](int x, int y, int w, int h,
                        const char* label, const char* value,
                        Color bg2, Color lc, Color vc, int valueFontSize) {
        DrawRectangle(x, y, w, h, bg2);
        DrawRectangleLines(x, y, w, h, cellBorder);
        DrawText(label, x + 8, y + 8, 14, lc);
        int vw = MeasureText(value, valueFontSize);
        DrawText(value, x + w - vw - 12, y + h - valueFontSize - 12, valueFontSize, vc);
    };

    // -- Left column --
    char buf[32];

    // Cell 0: RPM
    snprintf(buf, sizeof(buf), "%.0f", rpm);
    drawCell(0, topY + cellH*0, leftW, cellH,
             "RPM", buf, cellBg, labelColor, valueColor, 36);

    // Cell 1: Speed
    snprintf(buf, sizeof(buf), "%.1f", speed);
    drawCell(0, topY + cellH*1, leftW, cellH,
             "SPEED (km/h)", buf, cellBg, labelColor, valueColor, 36);

    // Cell 2: Throttle
    snprintf(buf, sizeof(buf), "%.1f", throttle);
    drawCell(0, topY + cellH*2, leftW, cellH,
             "THROTTLE (%)", buf, cellBg, labelColor, valueColor, 36);

    // Cell 3: Voltage
    snprintf(buf, sizeof(buf), "%.1f", voltage);
    drawCell(0, topY + cellH*3, leftW, cellH,
             "VOLTAGE (V)", buf, cellBg, labelColor, valueColor, 36);

    // -- Center column — primary value (coolant temp, large) --
    // Top small cell: MAF
    snprintf(buf, sizeof(buf), "%.1f", maf);
    drawCell(centerX, topY, centerW, cellH,
             "MAF (g/s)", buf, cellBg, labelColor, primaryColor, 28);

    // Large center cell spanning 2 rows: coolant temp
    int bigH = cellH * 2;
    DrawRectangle(centerX, topY + cellH, centerW, bigH, {20, 20, 0, 255});
    DrawRectangleLines(centerX, topY + cellH, centerW, bigH, cellBorder);
    snprintf(buf, sizeof(buf), "%.0f", coolant);
    int bigFontSize = 80;
    int bw = MeasureText(buf, bigFontSize);
    DrawText(buf, centerX + (centerW - bw) / 2,
             topY + cellH + (bigH - bigFontSize) / 2, bigFontSize, primaryColor);
    DrawText("COOLANT", centerX + 8, topY + cellH + 8, 13, labelColor);

    // Bottom small cell: oil temp
    snprintf(buf, sizeof(buf), "%.0f", oilTemp);
    Color oilBg = (oilTemp > 120.0f) ? warnColor : cellBg;
    drawCell(centerX, topY + cellH*3, centerW, cellH,
             "OIL (C)", buf, oilBg, labelColor, valueColor, 28);

    // -- Right column — 2x2 grid --
    int rCellW = rightW / 2;
    int rCellH = gridH / 2;

    // Top-left: intake temp
    snprintf(buf, sizeof(buf), "%.1f", intakeTemp);
    drawCell(rightX,          topY,          rCellW, rCellH,
             "INTAKE TEMP (C)", buf, cellBg, labelColor, valueColor, 36);

    // Top-right: oil temp (mirrored large display)
    snprintf(buf, sizeof(buf), "%.1f", oilTemp);
    drawCell(rightX + rCellW, topY,          rCellW, rCellH,
             "OIL TEMP (C)", buf, cellBg, labelColor, valueColor, 36);

    // Bottom-left: MAF
    snprintf(buf, sizeof(buf), "%.2f", maf);
    drawCell(rightX,          topY + rCellH, rCellW, rCellH,
             "MAF (g/s)", buf, cellBg, labelColor, valueColor, 36);

    // Bottom-right: throttle
    snprintf(buf, sizeof(buf), "%.1f%%", throttle);
    drawCell(rightX + rCellW, topY + rCellH, rCellW, rCellH,
             "THROTTLE", buf, cellBg, labelColor, valueColor, 36);

    // ----------------------------------------------------------------
    // Bottom bar — two buttons
    // ----------------------------------------------------------------
    DrawRectangle(0, botY, W, H - botY, barColor);

    // Left button
    DrawRectangle(8, botY + 8, 160, 36, warnColor);
    DrawText("SETTINGS", 28, botY + 18, 16, WHITE);

    // Right button
    DrawRectangle(W - 168, botY + 8, 160, 36, {0, 60, 120, 255});
    DrawText("MAIN MENU", W - 152, botY + 18, 16, WHITE);
}

int OBDView::logic() {


    return 0;
}

void OBDView::fetchEvents() {

}