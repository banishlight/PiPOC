#include <views/OBDView.hpp>
#include <views/MainView.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <ViewHandler.hpp>
#include <raylib.h>

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
                    case OBDEvent::OBDType::RPM:         _rpm      = obd.value; break;
                    case OBDEvent::OBDType::CoolantTemp: _coolant  = obd.value; break;
                    case OBDEvent::OBDType::Speed:       _speed    = obd.value; break;
                    case OBDEvent::OBDType::ThrottlePos: _throttle = obd.value; break;
                }
                break;
            }

            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                if (input->inputType != InputEvent::InputType::TAP) break;

                // MAIN MENU button — bottom right
                int botY = DISPLAY_H - 52;
                Rectangle mainMenuBtn = {
                    (float)(DISPLAY_W - 168),
                    (float)(botY + 8),
                    160.0f,
                    36.0f
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

void OBDView::draw() {
    // Formats a value if present, otherwise "--"
    auto fmt = [](char* buf, size_t sz, std::optional<float> val, const char* format) {
        if (!val.has_value())
            snprintf(buf, sz, "--");
        else
            snprintf(buf, sz, format, val.value());
    };

    // -- Colors --
    Color bg         = {10,  10,  10,  255};
    Color cellBg     = {20,  20,  20,  255};
    Color cellBorder = {40,  40,  40,  255};
    Color labelColor = {0,   180, 100, 255};
    Color valueColor = {255, 255, 255, 255};
    Color warnColor  = {180, 30,  30,  255};
    Color barColor   = {30,  30,  30,  255};

    auto drawCell = [&](int x, int y, int w, int h,
                        const char* label, const char* value,
                        Color bg2, Color lc, Color vc, int valueFontSize) {
        DrawRectangle(x, y, w, h, bg2);
        DrawRectangleLines(x, y, w, h, cellBorder);
        DrawTextEx(Assets::mainFont, label,
                   {(float)(x + 8), (float)(y + 8)}, 14, 1, lc);
        int vw = (int)MeasureTextEx(Assets::mainFont, value, (float)valueFontSize, 1).x;
        DrawTextEx(Assets::mainFont, value,
                   {(float)(x + w - vw - 12), (float)(y + h - valueFontSize - 12)},
                   (float)valueFontSize, 1, vc);
    };

    char buf[32];

    // Background
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, bg);

    // Top bar
    DrawRectangle(0, 0, DISPLAY_W, 36, barColor);
    DrawTextEx(Assets::mainFont, "OBD LIVE DATA",
               {(float)(DISPLAY_W / 2 - 80), 10}, 16, 1, GRAY);

    // Layout
    int topY  = 36;
    int botY  = DISPLAY_H - 52;
    int gridH = botY - topY;
    int cellH = gridH / 2;
    int cellW = DISPLAY_W / 2;

    // -- RPM --
    fmt(buf, sizeof(buf), _rpm, "%.0f");
    drawCell(0, topY, cellW, cellH,
             "RPM", buf, cellBg, labelColor, valueColor, 48);

    // -- SPEED --
    fmt(buf, sizeof(buf), _speed, "%.0f");
    drawCell(cellW, topY, cellW, cellH,
             "SPEED (km/h)", buf, cellBg, labelColor, valueColor, 48);

    // -- COOLANT TEMP --
    fmt(buf, sizeof(buf), _coolant, "%.1f");
    Color coolantBg = (_coolant.has_value() && _coolant.value() > 105.0f) ? warnColor : cellBg;
    drawCell(0, topY + cellH, cellW, cellH,
             "COOLANT (C)", buf, coolantBg, labelColor, valueColor, 48);

    // -- THROTTLE --
    fmt(buf, sizeof(buf), _throttle, "%.1f");
    drawCell(cellW, topY + cellH, cellW, cellH,
             "THROTTLE (%)", buf, cellBg, labelColor, valueColor, 48);

    // Bottom bar
    DrawRectangle(0, botY, DISPLAY_W, DISPLAY_H - botY, barColor);

    DrawRectangle(DISPLAY_W - 168, botY + 8, 160, 36, {0, 60, 120, 255});
    DrawTextEx(Assets::mainFont, "MAIN MENU",
               {(float)(DISPLAY_W - 152), (float)(botY + 18)}, 16, 1, WHITE);
}