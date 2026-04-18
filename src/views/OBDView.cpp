#include <views/OBDView.hpp>
#include <views/MainView.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <ViewHandler.hpp>
#include <raylib.h>
#include <cstdio>
#include <cstring>
#include <ctime>

// Colours
static constexpr Color C_BG         = {10,  10,  10,  255};
static constexpr Color C_BG2        = {15,  15,  15,  255};
static constexpr Color C_BG_CELL    = {17,  17,  17,  255};
static constexpr Color C_BG_COOLANT = {20,  20,  0,   255};
static constexpr Color C_BG_WARN    = {26,  8,   8,   255};
static constexpr Color C_GRID       = {20,  20,  20,  255};
static constexpr Color C_BORDER     = {26,  26,  26,  255};
static constexpr Color C_LABEL      = {0,   180, 100, 255};
static constexpr Color C_VALUE      = {255, 255, 255, 255};
static constexpr Color C_UNIT       = {68,  68,  68,  255};
static constexpr Color C_YELLOW     = {220, 220, 0,   255};
static constexpr Color C_RED_ACCENT = {255, 40,  0,   255};
static constexpr Color C_GREEN_BAR  = {0,   180, 100, 255};
static constexpr Color C_YELLOW_BAR = {220, 220, 0,   255};
static constexpr Color C_RED_BAR    = {180, 30,  30,  255};
static constexpr Color C_DIM        = {85,  85,  85,  255};
static constexpr Color C_MENU_BG    = {0,   60,  120, 255};
static constexpr Color C_MENU_TEXT  = {170, 200, 240, 255};

// Layout
static constexpr int TOPBAR_H  = 32;
static constexpr int BOTBAR_H  = 32;
static constexpr int BAR_H     = 3;
static constexpr int GAP       = 2;
static constexpr int PAD       = 8;

OBDView::OBDView() {}
OBDView::~OBDView() {}
void OBDView::start() {}
void OBDView::close() {}

void OBDView::fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        switch (e->type) {

            case ViewEvent::Type::OBD: {
                auto& obd = static_cast<OBDEvent&>(*e);
                switch (obd.obdType) {
                    case OBDEvent::OBDType::RPM:         _rpm        = obd.value; break;
                    case OBDEvent::OBDType::CoolantTemp: _coolant    = obd.value; break;
                    case OBDEvent::OBDType::Speed:       _speed      = obd.value; break;
                    case OBDEvent::OBDType::ThrottlePos: _throttle   = obd.value; break;
                    case OBDEvent::OBDType::OilTemp:     _oilTemp    = obd.value; break;
                    case OBDEvent::OBDType::IntakeTemp:  _intakeTemp = obd.value; break;
                    case OBDEvent::OBDType::MAF:         _maf        = obd.value; break;
                }
                break;
            }

            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType != InputEvent::InputType::TAP) break;

                // MAIN MENU button
                Rectangle mainMenuBtn = {
                    (float)(DISPLAY_W - 130),
                    (float)(DISPLAY_H - BOTBAR_H + 4),
                    120.0f,
                    (float)(BOTBAR_H - 8)
                };
                if (CheckCollisionPointRec({input->x, input->y}, mainMenuBtn)) {
                    ViewHandler::getInstance().switchView(std::make_unique<MainView>());
                    return;
                }
                break;
            }

            default: break;
        }
    }
}

int OBDView::logic() {
    fetchEvents();
    return 0;
}

// Formats an optional float value or returns "--" if no data
static void fmtVal(char* buf, size_t sz, std::optional<float> val, const char* format) {
    if (!val.has_value())
        snprintf(buf, sz, "--");
    else
        snprintf(buf, sz, format, val.value());
}

// Clamps a float to [0, 1]
static float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

void OBDView::drawCell(int x, int y, int w, int h,
                       const char* label, const char* value, const char* unit,
                       Color bg, Color labelCol, Color valueCol,
                       float barPct, Color barCol, bool hasBar) const {
    // Background
    DrawRectangle(x, y, w, h, bg);

    // Label — top left
    DrawTextEx(Assets::catFont16, label,
               {(float)(x + PAD), (float)(y + PAD)},
               11, 2, labelCol);

    // Value — vertically centred
    int fontSize = 28;
    Vector2 vSize = MeasureTextEx(Assets::catFont24, value, fontSize, 1);
    int vx = x + PAD;
    int vy = y + (h - fontSize) / 2;
    DrawTextEx(Assets::catFont24, value, {(float)vx, (float)vy}, fontSize, 1, valueCol);

    // Unit — below value
    DrawTextEx(Assets::catFont16, unit,
               {(float)(x + PAD), (float)(y + vy + fontSize + 2)},
               10, 1, C_UNIT);

    // Progress bar at bottom
    if (hasBar) {
        DrawRectangle(x, y + h - BAR_H, w, BAR_H, C_BORDER);
        int barW = (int)(clamp01(barPct) * w);
        DrawRectangle(x, y + h - BAR_H, barW, BAR_H, barCol);
    }
}

void OBDView::drawTopBar() const {
    DrawRectangle(0, 0, DISPLAY_W, TOPBAR_H, C_BG2);
    DrawRectangle(0, TOPBAR_H - 1, DISPLAY_W, 1, C_BORDER);

    // Left — car model
    DrawTextEx(Assets::catFont16, "MAZDA RX-8  13B-MSP",
               {(float)PAD, 10}, 11, 2, C_DIM);

    // Right — status indicators + clock
    char clockBuf[16];
    time_t now = time(nullptr);
    strftime(clockBuf, sizeof(clockBuf), "%H:%M:%S", localtime(&now));

    int cw = (int)MeasureTextEx(Assets::catFont16, clockBuf, 11, 1).x;
    DrawTextEx(Assets::catFont16, clockBuf,
               {(float)(DISPLAY_W - cw - PAD), 10}, 11, 1, C_DIM);

    DrawRectangle(DISPLAY_W - cw - PAD - 12, 8, 1, 16, C_BORDER);
    DrawTextEx(Assets::catFont16, "BT CONNECTED",
               {(float)(DISPLAY_W - cw - PAD - 100), 10}, 11, 2, C_DIM);

    DrawRectangle(DISPLAY_W - cw - PAD - 112, 8, 1, 16, C_BORDER);
    DrawTextEx(Assets::catFont16, "ECU ONLINE",
               {(float)(DISPLAY_W - cw - PAD - 198), 10}, 11, 2, C_DIM);
}

void OBDView::drawBottomBar() const {
    int y = DISPLAY_H - BOTBAR_H;
    DrawRectangle(0, y, DISPLAY_W, BOTBAR_H, C_BG2);
    DrawRectangle(0, y, DISPLAY_W, 1, C_BORDER);

    // BT dot + device name
    DrawCircle(PAD + 4, y + BOTBAR_H / 2, 3, C_RED_ACCENT);
    DrawTextEx(Assets::catFont16, "CUBA'S IPHONE",
               {(float)(PAD + 14), (float)(y + 10)}, 11, 2, C_DIM);

    // MAIN MENU button — right side
    int btnW = 120, btnH = BOTBAR_H - 8;
    int bx = DISPLAY_W - btnW - PAD;
    int by = y + 4;
    DrawRectangle(bx, by, btnW, btnH, C_MENU_BG);
    DrawTextEx(Assets::catFont16, "MAIN MENU",
               {(float)(bx + 10), (float)(by + 6)}, 11, 2, C_MENU_TEXT);
}

void OBDView::draw() {
    char buf[32];

    // Background + grid colour
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, C_GRID);

    drawTopBar();
    drawBottomBar();

    // Grid area
    int gridY = TOPBAR_H + GAP;
    int gridH = DISPLAY_H - TOPBAR_H - BOTBAR_H - GAP * 2;
    int gridW = DISPLAY_W - GAP * 2;

    // Column widths — matching 2fr 1.2fr 1fr 1fr
    int col0W = (int)(gridW * 2.0f   / 5.2f);
    int col1W = (int)(gridW * 1.2f   / 5.2f);
    int col2W = (int)(gridW * 1.0f   / 5.2f);
    int col3W = gridW - col0W - col1W - col2W;

    int col0X = GAP;
    int col1X = col0X + col0W + GAP;
    int col2X = col1X + col1W + GAP;
    int col3X = col2X + col2W + GAP;

    // Row heights — 3 equal rows
    int rowH = (gridH - GAP * 2) / 3;

    int row0Y = gridY;
    int row1Y = row0Y + rowH + GAP;
    int row2Y = row1Y + rowH + GAP;

    // ---- RPM — hero, spans rows 0+1, col 0 ----
    int heroH = rowH * 2 + GAP;
    fmtVal(buf, sizeof(buf), _rpm, "%.0f");
    // Background
    DrawRectangle(col0X, row0Y, col0W, heroH, C_BG_CELL);
    // Red accent top border
    DrawRectangle(col0X, row0Y, col0W, 2, C_RED_ACCENT);
    // Label
    DrawTextEx(Assets::catFont16, "RPM",
               {(float)(col0X + PAD), (float)(row0Y + PAD)}, 11, 2, C_LABEL);
    // Value — large, centred
    int rpmFont = 64;
    Vector2 rpmSize = MeasureTextEx(Assets::catFont52, buf, rpmFont, 1);
    DrawTextEx(Assets::catFont52, buf,
               {(float)(col0X + PAD), (float)(row0Y + (heroH - rpmFont) / 2)},
               rpmFont, 1, C_YELLOW);
    // Unit
    DrawTextEx(Assets::catFont16, "REVS / MIN",
               {(float)(col0X + PAD), (float)(row0Y + heroH - 22)}, 10, 1, C_UNIT);
    // Bar
    float rpmPct = _rpm.has_value() ? clamp01(_rpm.value() / BAR_RPM_MAX) : 0.0f;
    DrawRectangle(col0X, row0Y + heroH - BAR_H, col0W, BAR_H, C_BORDER);
    DrawRectangle(col0X, row0Y + heroH - BAR_H, (int)(rpmPct * col0W), BAR_H, C_YELLOW_BAR);

    // ---- COOLANT — hero, spans rows 0+1, col 1 ----
    fmtVal(buf, sizeof(buf), _coolant, "%.0f");
    bool coolantWarn = _coolant.has_value() && _coolant.value() > WARN_COOLANT_C;
    Color coolantBg  = coolantWarn ? C_BG_WARN : C_BG_COOLANT;
    Color coolantBar = coolantWarn ? C_RED_BAR : C_GREEN_BAR;
    DrawRectangle(col1X, row0Y, col1W, heroH, coolantBg);
    DrawRectangle(col1X, row0Y, col1W, 2, C_RED_ACCENT);
    DrawTextEx(Assets::catFont16, "COOLANT",
               {(float)(col1X + PAD), (float)(row0Y + PAD)}, 11, 2, C_LABEL);
    int coolFont = 52;
    DrawTextEx(Assets::catFont52, buf,
               {(float)(col1X + PAD), (float)(row0Y + (heroH - coolFont) / 2)},
               coolFont, 1, C_YELLOW);
    DrawTextEx(Assets::catFont16, "DEGREES C",
               {(float)(col1X + PAD), (float)(row0Y + heroH - 22)}, 10, 1, C_UNIT);
    float coolPct = _coolant.has_value() ? clamp01(_coolant.value() / BAR_COOLANT_MAX) : 0.0f;
    DrawRectangle(col1X, row0Y + heroH - BAR_H, col1W, BAR_H, C_BORDER);
    DrawRectangle(col1X, row0Y + heroH - BAR_H, (int)(coolPct * col1W), BAR_H, coolantBar);

    // ---- SPEED — row 0, col 2 ----
    fmtVal(buf, sizeof(buf), _speed, "%.0f");
    DrawRectangle(col2X, row0Y, col2W, rowH, C_BG_CELL);
    DrawRectangle(col2X, row0Y, col2W, 2, C_GREEN_BAR);
    DrawTextEx(Assets::catFont16, "SPEED",
               {(float)(col2X + PAD), (float)(row0Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col2X + PAD), (float)(row0Y + (rowH - 28) / 2)}, 28, 1, C_VALUE);
    DrawTextEx(Assets::catFont16, "KM/H",
               {(float)(col2X + PAD), (float)(row0Y + rowH - 22)}, 10, 1, C_UNIT);
    float speedPct = _speed.has_value() ? clamp01(_speed.value() / BAR_SPEED_MAX) : 0.0f;
    DrawRectangle(col2X, row0Y + rowH - BAR_H, col2W, BAR_H, C_BORDER);
    DrawRectangle(col2X, row0Y + rowH - BAR_H, (int)(speedPct * col2W), BAR_H, C_GREEN_BAR);

    // ---- THROTTLE — row 0, col 3 ----
    fmtVal(buf, sizeof(buf), _throttle, "%.1f");
    DrawRectangle(col3X, row0Y, col3W, rowH, C_BG_CELL);
    DrawRectangle(col3X, row0Y, col3W, 2, C_GREEN_BAR);
    DrawTextEx(Assets::catFont16, "THROTTLE",
               {(float)(col3X + PAD), (float)(row0Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col3X + PAD), (float)(row0Y + (rowH - 28) / 2)}, 28, 1, C_VALUE);
    DrawTextEx(Assets::catFont16, "PERCENT",
               {(float)(col3X + PAD), (float)(row0Y + rowH - 22)}, 10, 1, C_UNIT);
    float thrPct = _throttle.has_value() ? clamp01(_throttle.value() / BAR_THROTTLE_MAX) : 0.0f;
    DrawRectangle(col3X, row0Y + rowH - BAR_H, col3W, BAR_H, C_BORDER);
    DrawRectangle(col3X, row0Y + rowH - BAR_H, (int)(thrPct * col3W), BAR_H, C_GREEN_BAR);

    // ---- MAF — row 1, col 2 ----
    fmtVal(buf, sizeof(buf), _maf, "%.1f");
    DrawRectangle(col2X, row1Y, col2W, rowH, C_BG_CELL);
    DrawTextEx(Assets::catFont16, "MAF",
               {(float)(col2X + PAD), (float)(row1Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col2X + PAD), (float)(row1Y + (rowH - 28) / 2)}, 28, 1, C_VALUE);
    DrawTextEx(Assets::catFont16, "G/SEC",
               {(float)(col2X + PAD), (float)(row1Y + rowH - 22)}, 10, 1, C_UNIT);
    float mafPct = _maf.has_value() ? clamp01(_maf.value() / BAR_MAF_MAX) : 0.0f;
    DrawRectangle(col2X, row1Y + rowH - BAR_H, col2W, BAR_H, C_BORDER);
    DrawRectangle(col2X, row1Y + rowH - BAR_H, (int)(mafPct * col2W), BAR_H, C_GREEN_BAR);

    // ---- VOLTAGE — row 1, col 3 ----
    fmtVal(buf, sizeof(buf), _voltage, "%.1f");
    DrawRectangle(col3X, row1Y, col3W, rowH, C_BG_CELL);
    DrawTextEx(Assets::catFont16, "VOLTAGE",
               {(float)(col3X + PAD), (float)(row1Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col3X + PAD), (float)(row1Y + (rowH - 28) / 2)}, 28, 1, C_VALUE);
    DrawTextEx(Assets::catFont16, "VOLTS",
               {(float)(col3X + PAD), (float)(row1Y + rowH - 22)}, 10, 1, C_UNIT);

    // ---- OIL TEMP — row 2, col 0 ----
    fmtVal(buf, sizeof(buf), _oilTemp, "%.0f");
    bool oilWarn   = _oilTemp.has_value() && _oilTemp.value() > WARN_OIL_C;
    Color oilValue = oilWarn ? C_RED_ACCENT : C_VALUE;
    DrawRectangle(col0X, row2Y, col0W, rowH, C_BG_CELL);
    DrawTextEx(Assets::catFont16, "OIL TEMP",
               {(float)(col0X + PAD), (float)(row2Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col0X + PAD), (float)(row2Y + (rowH - 28) / 2)}, 28, 1, oilValue);
    DrawTextEx(Assets::catFont16, "DEGREES C",
               {(float)(col0X + PAD), (float)(row2Y + rowH - 22)}, 10, 1, C_UNIT);
    float oilPct = _oilTemp.has_value() ? clamp01(_oilTemp.value() / BAR_OIL_MAX) : 0.0f;
    DrawRectangle(col0X, row2Y + rowH - BAR_H, col0W, BAR_H, C_BORDER);
    DrawRectangle(col0X, row2Y + rowH - BAR_H, (int)(oilPct * col0W), BAR_H, C_YELLOW_BAR);

    // ---- INTAKE AIR — row 2, col 1 ----
    fmtVal(buf, sizeof(buf), _intakeTemp, "%.1f");
    DrawRectangle(col1X, row2Y, col1W, rowH, C_BG_CELL);
    DrawTextEx(Assets::catFont16, "INTAKE AIR",
               {(float)(col1X + PAD), (float)(row2Y + PAD)}, 11, 2, C_LABEL);
    DrawTextEx(Assets::catFont24, buf,
               {(float)(col1X + PAD), (float)(row2Y + (rowH - 28) / 2)}, 28, 1, C_VALUE);
    DrawTextEx(Assets::catFont16, "DEGREES C",
               {(float)(col1X + PAD), (float)(row2Y + rowH - 22)}, 10, 1, C_UNIT);

    // ---- Empty cells — row 2, cols 2+3 ----
    DrawRectangle(col2X, row2Y, col2W, rowH, C_BG);
    DrawRectangle(col3X, row2Y, col3W, rowH, C_BG);
}