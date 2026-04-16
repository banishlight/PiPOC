#include <views/MainView.hpp>
#include <views/OBDView.hpp>
#include <views/MusicView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <raylib.h>
#include <cstring>
#include <ctime>

// ============================================================================
// Layout constants
// ============================================================================
static constexpr int   W               = 1024;
static constexpr int   H               = 600;
static constexpr int   TOPBAR_H        = 36;
static constexpr int   BOTBAR_H        = 40;
static constexpr Color BG              = {10,  10,  10,  255};
static constexpr Color BG2             = {15,  15,  15,  255};
static constexpr Color BORDER          = {26,  26,  26,  255};
static constexpr Color RED_ACCENT      = {255, 40,  0,   255};
static constexpr Color TEXT_DIM        = {80,  80,  80,  255};
static constexpr Color TEXT_MID        = {140, 140, 140, 255};
static constexpr Color TEXT_BRIGHT     = {220, 220, 220, 255};

// Button layout
static constexpr int   BTN_COUNT       = 3;
static constexpr int   BTN_W           = 200;
static constexpr int   BTN_H           = 90;
static constexpr int   BTN_GAP         = 2;
static constexpr int   BTN_GRID_W      = BTN_COUNT * BTN_W + (BTN_COUNT - 1) * BTN_GAP;
static constexpr int   BTN_GRID_X      = (W - BTN_GRID_W) / 2;
static constexpr int   BTN_GRID_Y      = (H - BTN_H) / 2 + 40;

static const char* BTN_LABELS[]    = { "ECU",   "AUDIO",  "MAPS"  };
static const char* BTN_SUBLABELS[] = { "LIVE DATA", "MEDIA PLAYER", "NAVIGATION" };

// ============================================================================
MainView::MainView() {}
MainView::~MainView() {}

void MainView::start() {}
void MainView::close() {}

// ============================================================================
int MainView::logic() {
    float dt = GetFrameTime();
    _fetchEvents();

    if (_transitioning) {
        _transitionTimer += dt;
        if (_transitionTimer >= kTransitionDuration && _pendingSwitch) {
            _pendingSwitch();
        }
    }
    return 0;
}

// ============================================================================
void MainView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        if (e->type != ViewEvent::Type::INPUT) continue;
        auto* input = static_cast<InputEvent*>(e.get());
        if (input->inputType != InputEvent::InputType::TAP) continue;
        if (_transitioning) continue;

        for (int i = 0; i < BTN_COUNT; i++) {
            int bx = BTN_GRID_X + i * (BTN_W + BTN_GAP);
            if (CheckCollisionPointRec({input->x, input->y},
                {(float)bx, (float)BTN_GRID_Y, (float)BTN_W, (float)BTN_H})) {
                _hoveredBtn     = i;
                _transitioning  = true;
                _transitionTimer = 0.0f;

                switch (i) {
                    case 0: _pendingSwitch = [](){
                        ViewHandler::getInstance().switchView(std::make_unique<OBDView>());
                    }; break;
                    case 1: _pendingSwitch = [](){
                        ViewHandler::getInstance().switchView(std::make_unique<MusicView>());
                    }; break;
                    case 2: /* Maps — not yet implemented */ break;
                }
                break;
            }
        }
    }
}

// ============================================================================
void MainView::draw() {
    Font& f = Assets::mainFont;

    // Background
    DrawRectangle(0, 0, W, H, BG);

    // Corner brackets
    int bSize = 20, bThick = 2;
    // TL
    DrawRectangle(8, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, 8, bThick, bSize, RED_ACCENT);
    // TR
    DrawRectangle(W - 8 - bSize, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(W - 8 - bThick, 8, bThick, bSize, RED_ACCENT);
    // BL
    DrawRectangle(8, H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, H - 8 - bSize, bThick, bSize, RED_ACCENT);
    // BR
    DrawRectangle(W - 8 - bSize, H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(W - 8 - bThick, H - 8 - bSize, bThick, bSize, RED_ACCENT);

    // Top bar
    DrawRectangle(0, 0, W, TOPBAR_H, BG2);
    DrawRectangle(0, TOPBAR_H - 1, W, 1, BORDER);
    DrawTextEx(f, "MAZDA RX-8  13B-MSP", {82, 11}, 11, 3, TEXT_DIM);

    // Top bar right — status + clock
    char clockBuf[16];
    time_t now = time(nullptr);
    strftime(clockBuf, sizeof(clockBuf), "%H:%M:%S", localtime(&now));
    int cw = (int)MeasureTextEx(f, clockBuf, 11, 1).x;
    DrawTextEx(f, clockBuf, {(float)(W - cw - 16), 12}, 11, 1, TEXT_DIM);
    DrawRectangle(W - cw - 32, 8, 1, 20, BORDER);
    DrawTextEx(f, "BT  CONNECTED", {(float)(W - cw - 130), 12}, 11, 2, TEXT_DIM);
    DrawRectangle(W - cw - 146, 8, 1, 20, BORDER);
    DrawTextEx(f, "ECU  ONLINE", {(float)(W - cw - 240), 12}, 11, 2, TEXT_DIM);

    // Title block
    int titleY = TOPBAR_H + 60;
    const char* title = "RX-8";
    int tw = (int)MeasureTextEx(f, title, 52, 12).x;
    DrawTextEx(f, title, {(float)((W - tw) / 2), (float)titleY}, 52, 12, TEXT_BRIGHT);

    // Red accent line
    DrawRectangle((W - 120) / 2, titleY + 62, 120, 2, RED_ACCENT);

    // Sub title
    const char* sub = "SELECT SYSTEM";
    int sw = (int)MeasureTextEx(f, sub, 11, 8).x;
    DrawTextEx(f, sub, {(float)((W - sw) / 2), (float)(titleY + 72)}, 11, 8, TEXT_DIM);

    // Nav buttons
    for (int i = 0; i < BTN_COUNT; i++) {
        int bx = BTN_GRID_X + i * (BTN_W + BTN_GAP);
        int by = BTN_GRID_Y;

        // Button fill animation
        Color btnBg = BG2;
        Color topLine = BORDER;
        Color labelColor = TEXT_MID;

        if (_transitioning && _hoveredBtn == i) {
            float t = _transitionTimer / kTransitionDuration;
            t = t > 1.0f ? 1.0f : t;
            // Fill red from left to right
            DrawRectangle(bx, by, BTN_W, BTN_H, BG2);
            DrawRectangle(bx, by, (int)(BTN_W * t), BTN_H, RED_ACCENT);
            topLine    = RED_ACCENT;
            labelColor = WHITE;
        } else {
            DrawRectangle(bx, by, BTN_W, BTN_H, btnBg);
        }

        DrawRectangle(bx, by, BTN_W, 2, topLine);
        DrawRectangle(bx, by, BTN_W, BTN_H, {0, 0, 0, 0}); // border
        DrawRectangleLines(bx, by, BTN_W, BTN_H, BORDER);

        // Label
        int lw = (int)MeasureTextEx(f, BTN_LABELS[i], 18, 4).x;
        DrawTextEx(f, BTN_LABELS[i], {(float)(bx + (BTN_W - lw) / 2), (float)(by + 28)}, 18, 4, labelColor);

        // Sub label
        int slw = (int)MeasureTextEx(f, BTN_SUBLABELS[i], 9, 2).x;
        DrawTextEx(f, BTN_SUBLABELS[i], {(float)(bx + (BTN_W - slw) / 2), (float)(by + 54)}, 9, 2,
                   (_transitioning && _hoveredBtn == i) ? (Color){255,200,200,255} : TEXT_DIM);
    }

    // Bottom bar
    DrawRectangle(0, H - BOTBAR_H, W, BOTBAR_H, BG2);
    DrawRectangle(0, H - BOTBAR_H, W, 1, BORDER);

    // BT dot + device name
    DrawCircle(22, H - BOTBAR_H + 20, 3, RED_ACCENT);
    DrawTextEx(f, "CUBA'S IPHONE", {34, (float)(H - BOTBAR_H + 13)}, 10, 2, TEXT_DIM);

    // Version
    const char* ver = "v0.1.0  PIPOC";
    int vw = (int)MeasureTextEx(f, ver, 10, 2).x;
    DrawTextEx(f, ver, {(float)(W - vw - 16), (float)(H - BOTBAR_H + 13)}, 10, 2, {34, 34, 34, 255});
}