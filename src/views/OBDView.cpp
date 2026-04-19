#include <views/OBDView.hpp>
#include <views/MainView.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <ViewHandler.hpp>
#include <Sensor.hpp>
#include <raylib.h>
#include <cstdio>

// Font sizes — must match the sizes fonts were loaded at in Assets::load()
static constexpr float FONT_16 = 16.0f;
static constexpr float FONT_24 = 24.0f;
static constexpr float FONT_52 = 52.0f;
static constexpr float FONT_88 = 88.0f;
// Colours
static constexpr Color C_BG         = {10,  10,  10,  255};
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

// Layout
static constexpr int TOPBAR_H = TopBar::HEIGHT;
static constexpr int BOTBAR_H = BottomBar::HEIGHT;
static constexpr int BAR_H    = 3;
static constexpr int GAP      = 2;
static constexpr int PAD      = 8;

OBDView::OBDView() {
    _rpm.setMax(9500.0f);
    _coolant.setMax(120.0f);
    _speed.setMax(120.0f);
    _throttle.setMax(100.0f);
    _oilTemp.setMax(130.0f);
    _intakeTemp.setMax(60.0f);
    _maf.setMax(20.0f);
    _voltage.setMax(16.0f);
}
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
                    case OBDEvent::OBDType::RPM:         _rpm.setValue(obd.value); break;
                    case OBDEvent::OBDType::CoolantTemp: _coolant.setValue(obd.value); break;
                    case OBDEvent::OBDType::Speed:       _speed.setValue(obd.value); break;
                    case OBDEvent::OBDType::ThrottlePos: _throttle.setValue(obd.value); break;
                    case OBDEvent::OBDType::OilTemp:     _oilTemp.setValue(obd.value); break;
                    case OBDEvent::OBDType::IntakeTemp:  _intakeTemp.setValue(obd.value); break;
                    case OBDEvent::OBDType::MAF:         _maf.setValue(obd.value); break;
                }
                break;
            }

            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType != InputEvent::InputType::TAP) break;
                _bottomBar.handleEvent(*input);
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

static void fmtVal(char* buf, size_t sz, std::optional<float> val, const char* format) {
    if (!val.has_value())
        snprintf(buf, sz, "--");
    else
        snprintf(buf, sz, format, val.value());
}

// Draws a standard secondary cell using catFont24 for value, catFont16 for label/unit
static void drawSecondaryCell(int x, int y, int w, int h,
                              const char* label, const char* value, const char* unit,
                              Color bg, Color valueCol,
                              float barPct, Color barCol, bool hasBar) {
    DrawRectangle(x, y, w, h, bg);

    DrawTextEx(Assets::catFont16, label,
               {(float)(x + PAD), (float)(y + PAD)}, FONT_16, 1, C_LABEL);

    int vy = y + (h - (int)FONT_24) / 2;
    DrawTextEx(Assets::catFont24, value,
               {(float)(x + PAD), (float)vy}, FONT_24, 1, valueCol);

    DrawTextEx(Assets::catFont16, unit,
               {(float)(x + PAD), (float)(y + h - (int)FONT_16 - PAD)}, FONT_16, 1, C_UNIT);

    if (hasBar) {
        DrawRectangle(x, y + h - BAR_H, w, BAR_H, C_BORDER);
        DrawRectangle(x, y + h - BAR_H, (int)(barPct * w), BAR_H, barCol);
    }
}

void OBDView::draw() {
    char buf[32];

    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, C_GRID);
    _topBar.draw();
    _bottomBar.draw();

    // Grid area
    int gridY = TOPBAR_H + GAP;
    int gridH = DISPLAY_H - TOPBAR_H - BOTBAR_H - GAP * 2;
    int gridW = DISPLAY_W - GAP * 2;

    // Column widths — 2fr 1.2fr 1fr 1fr
    int col0W = (int)(gridW * 2.0f / 5.2f);
    int col1W = (int)(gridW * 1.2f / 5.2f);
    int col2W = (int)(gridW * 1.0f / 5.2f);
    int col3W = gridW - col0W - col1W - col2W;

    int col0X = GAP;
    int col1X = col0X + col0W + GAP;
    int col2X = col1X + col1W + GAP;
    int col3X = col2X + col2W + GAP;

    // Row heights — 3 equal rows
    int rowH  = (gridH - GAP * 2) / 3;
    int row0Y = gridY;
    int row1Y = row0Y + rowH + GAP;
    int row2Y = row1Y + rowH + GAP;
    int heroH = rowH * 2 + GAP;

    // ---- RPM — hero, rows 0+1, col 0 ----
    fmtVal(buf, sizeof(buf), _rpm.getValue(), "%.0f");
    DrawRectangle(col0X, row0Y, col0W, heroH, C_BG_CELL);
    DrawRectangle(col0X, row0Y, col0W, 2, C_RED_ACCENT);
    DrawTextEx(Assets::catFont16, "RPM",
               {(float)(col0X + PAD), (float)(row0Y + PAD)}, FONT_16, 1, C_LABEL);
    DrawTextEx(Assets::catFont88, buf,
               {(float)(col0X + PAD), (float)(row0Y + (heroH - (int)FONT_88) / 2)},
               FONT_88, 1, C_YELLOW);
    DrawTextEx(Assets::catFont16, "REVS / MIN",
               {(float)(col0X + PAD), (float)(row0Y + heroH - (int)FONT_16 - PAD)},
               FONT_16, 1, C_UNIT);
    float rpmPct = _rpm.getBarPct();
    DrawRectangle(col0X, row0Y + heroH - BAR_H, col0W, BAR_H, C_BORDER);
    DrawRectangle(col0X, row0Y + heroH - BAR_H, (int)(rpmPct * col0W), BAR_H, C_YELLOW_BAR);

    // ---- COOLANT — hero, rows 0+1, col 1 ----
    fmtVal(buf, sizeof(buf), _coolant.getValue(), "%.0f");
    bool coolantWarn = _coolant.getValue().has_value() && _coolant.getValue().value() > WARN_COOLANT_C;
    Color coolantBg  = coolantWarn ? C_BG_WARN : C_BG_COOLANT;
    Color coolantBar = coolantWarn ? C_RED_BAR  : C_GREEN_BAR;
    DrawRectangle(col1X, row0Y, col1W, heroH, coolantBg);
    DrawRectangle(col1X, row0Y, col1W, 2, C_RED_ACCENT);
    DrawTextEx(Assets::catFont16, "COOLANT",
               {(float)(col1X + PAD), (float)(row0Y + PAD)}, FONT_16, 1, C_LABEL);
    DrawTextEx(Assets::catFont88, buf,
               {(float)(col1X + PAD), (float)(row0Y + (heroH - (int)FONT_88) / 2)},
               FONT_88, 1, C_YELLOW);
    DrawTextEx(Assets::catFont16, "DEGREES C",
               {(float)(col1X + PAD), (float)(row0Y + heroH - (int)FONT_16 - PAD)},
               FONT_16, 1, C_UNIT);
    float coolPct = _coolant.getBarPct();
    DrawRectangle(col1X, row0Y + heroH - BAR_H, col1W, BAR_H, C_BORDER);
    DrawRectangle(col1X, row0Y + heroH - BAR_H, (int)(coolPct * col1W), BAR_H, coolantBar);

    // ---- SPEED — row 0, col 2 ----
    fmtVal(buf, sizeof(buf), _speed.getValue(), "%.0f");
    DrawRectangle(col2X, row0Y, col2W, rowH, C_BG_CELL);
    DrawRectangle(col2X, row0Y, col2W, 2, C_GREEN_BAR);
    drawSecondaryCell(col2X, row0Y, col2W, rowH, "SPEED", buf, "KM/H",
                      C_BG_CELL, C_VALUE,
                      _speed.getBarPct(),
                      C_GREEN_BAR, true);

    // ---- THROTTLE — row 0, col 3 ----
    fmtVal(buf, sizeof(buf), _throttle.getValue(), "%.1f");
    DrawRectangle(col3X, row0Y, col3W, 2, C_GREEN_BAR);
    drawSecondaryCell(col3X, row0Y, col3W, rowH, "THROTTLE", buf, "PERCENT",
                      C_BG_CELL, C_VALUE,
                      _throttle.getBarPct(),
                      C_GREEN_BAR, true);

    // ---- MAF — row 1, col 2 ----
    fmtVal(buf, sizeof(buf), _maf.getValue(), "%.1f");
    drawSecondaryCell(col2X, row1Y, col2W, rowH, "MAF", buf, "G/SEC",
                      C_BG_CELL, C_VALUE,
                      _maf.getBarPct(),
                      C_GREEN_BAR, true);

    // ---- VOLTAGE — row 1, col 3 ----
    fmtVal(buf, sizeof(buf), _voltage.getValue(), "%.1f");
    drawSecondaryCell(col3X, row1Y, col3W, rowH, "VOLTAGE", buf, "VOLTS",
                      C_BG_CELL, C_VALUE, 0.0f, C_GREEN_BAR, false);

    // ---- OIL TEMP — row 2, col 0 ----
    fmtVal(buf, sizeof(buf), _oilTemp.getValue(), "%.0f");
    bool  oilWarn = _oilTemp.getValue().has_value() && _oilTemp.getValue().value() > WARN_OIL_C;
    Color oilCol  = oilWarn ? C_RED_ACCENT : C_VALUE;
    drawSecondaryCell(col0X, row2Y, col0W, rowH, "OIL TEMP", buf, "DEGREES C",
                      C_BG_CELL, oilCol,
                      _oilTemp.getBarPct(),
                      C_YELLOW_BAR, true);

    // ---- INTAKE AIR — row 2, col 1 ----
    fmtVal(buf, sizeof(buf), _intakeTemp.getValue(), "%.1f");
    drawSecondaryCell(col1X, row2Y, col1W, rowH, "INTAKE AIR", buf, "DEGREES C",
                      C_BG_CELL, C_VALUE, 0.0f, C_GREEN_BAR, false);

    // ---- Empty cells — row 2, cols 2+3 ----
    DrawRectangle(col2X, row2Y, col2W, rowH, C_BG);
    DrawRectangle(col3X, row2Y, col3W, rowH, C_BG);
}