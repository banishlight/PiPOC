#include <views/SettingsView.hpp>
#include <views/MainView.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <raylib.h>

static constexpr int   W          = 1024;
static constexpr int   H          = 600;
static constexpr Color BG         = {10,  10,  10,  255};
static constexpr Color BG2        = {15,  15,  15,  255};
static constexpr Color BORDER     = {26,  26,  26,  255};
static constexpr Color RED_ACCENT = {255, 40,  0,   255};
static constexpr Color TEXT_DIM   = {80,  80,  80,  255};
static constexpr Color TEXT_BRIGHT= {220, 220, 220, 255};

SettingsView::SettingsView() {}
SettingsView::~SettingsView() {}

void SettingsView::start() {
    // Toggle fullscreen
    auto fsBtn = std::make_unique<Button>(412, 260, 200, 52, "FULLSCREEN");
    fsBtn->setOnClick([]() {
        ToggleFullscreen();
    });
    _widgets.push_back(std::move(fsBtn));

    // Back to main menu
    auto backBtn = std::make_unique<Button>(412, 320, 200, 52, "BACK");
    backBtn->setOnClick([]() {
        ViewHandler::getInstance().switchView(std::make_unique<MainView>());
    });
    _widgets.push_back(std::move(backBtn));
}

void SettingsView::close() {
    _widgets.clear();
}

int SettingsView::logic() {
    _fetchEvents();
    return 0;
}

void SettingsView::_fetchEvents() {
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

void SettingsView::draw() {
    DrawRectangle(0, 0, W, H, BG);

    // Top bar
    DrawRectangle(0, 0, W, 36, BG2);
    DrawRectangle(0, 35, W, 1, BORDER);
    DrawTextEx(Assets::catFont16, "SETTINGS", {16, 11}, 16, 3, TEXT_DIM);

    // Corner brackets
    int bSize = 20, bThick = 2;
    DrawRectangle(8, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(W - 8 - bSize, 8, bSize, bThick, RED_ACCENT);
    DrawRectangle(W - 8 - bThick, 8, bThick, bSize, RED_ACCENT);
    DrawRectangle(8, H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(8, H - 8 - bSize, bThick, bSize, RED_ACCENT);
    DrawRectangle(W - 8 - bSize, H - 8 - bThick, bSize, bThick, RED_ACCENT);
    DrawRectangle(W - 8 - bThick, H - 8 - bSize, bThick, bSize, RED_ACCENT);

    // Section label
    DrawTextEx(Assets::catFont16, "DISPLAY", {16, 52}, 16, 3, RED_ACCENT);
    DrawRectangle(16, 72, W - 32, 1, BORDER);

    // Widgets
    for (auto& w : _widgets) w->draw();

    // Bottom bar
    DrawRectangle(0, H - 40, W, 40, BG2);
    DrawRectangle(0, H - 40, W, 1, BORDER);
}