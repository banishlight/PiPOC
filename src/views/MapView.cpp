#include <views/MapView.hpp>
#include <views/MainView.hpp>
#include <ViewHandler.hpp>
#include <Config.hpp>
#include <Assets.hpp>
#include <raylib.h>

static constexpr Color BG         = {10,  10,  10,  255};
static constexpr Color BG2        = {15,  15,  15,  255};
static constexpr Color BORDER     = {26,  26,  26,  255};
static constexpr Color RED_ACCENT = {255, 40,  0,   255};
static constexpr Color TEXT_DIM   = {80,  80,  80,  255};
static constexpr Color TEXT_BRIGHT= {220, 220, 220, 255};

MapsView::MapsView() {}
MapsView::~MapsView() {}

void MapsView::start() {
    auto backBtn = std::make_unique<Button>(DISPLAY_W - 168, DISPLAY_H - 52, 160, 36, "BACK");
    backBtn->setOnClick([]() {
        ViewHandler::getInstance().switchView(std::make_unique<MainView>());
    });
    _widgets.push_back(std::move(backBtn));
}

void MapsView::close() {
    _widgets.clear();
}

int MapsView::logic() {
    _fetchEvents();
    return 0;
}

void MapsView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        if (e->type != ViewEvent::Type::INPUT) continue;
        auto* input = static_cast<InputEvent*>(e.get());
        if (input->inputType != InputEvent::InputType::TAP) continue;
        for (auto& w : _widgets) {
            if (w->handleEvent(*input)) break;
        }
    }
}

void MapsView::draw() {
    DrawRectangle(0, 0, DISPLAY_W, DISPLAY_H, BG);

    // Corner brackets
    int bSize = 20, bThick = 2;
    DrawRectangle(8, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bSize, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bThick, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(8, DISPLAY_H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, DISPLAY_H - 8 - bSize, bThick, bSize, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bSize, DISPLAY_H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(DISPLAY_W - 8 - bThick, DISPLAY_H - 8 - bSize, bThick, bSize, RED_ACCENT);

    // Top bar
    DrawRectangle(0, 0, DISPLAY_W, 36, BG2);
    DrawRectangle(0, 35, DISPLAY_W, 1, BORDER);
    DrawTextEx(Assets::catFont16, "NAVIGATION", {16, 10}, 16, 3, TEXT_DIM);

    // Placeholder
    const char* msg = "GPS HARDWARE NOT INSTALLED";
    int mw = (int)MeasureTextEx(Assets::catFont24, msg, 24, 3).x;
    DrawTextEx(Assets::catFont24, msg, {(float)((DISPLAY_W - mw) / 2), (float)(DISPLAY_H / 2 - 12)}, 24, 3, TEXT_DIM);

    // Bottom bar
    DrawRectangle(0, DISPLAY_H - 40, DISPLAY_W, 40, BG2);
    DrawRectangle(0, DISPLAY_H - 40, DISPLAY_W, 1, BORDER);

    for (auto& w : _widgets) w->draw();
}