#include <agents/DisplayAgent.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <Config.hpp>
#include <chrono>
#include <thread>
#include <raylib.h>
#include <memory>

DisplayAgent::DisplayAgent() {
    InitWindow(DISPLAY_W, DISPLAY_H, "PiPOC");
    SetTargetFPS(MAX_FPS);
    Assets::load();
    // ToggleFullscreen();
}

DisplayAgent::~DisplayAgent() {

}

void DisplayAgent::start() {
    _running = true;
    run();
}

void DisplayAgent::stop() {
    _running = false;
    Assets::unload();
}

void DisplayAgent::handleInput() {
    Vector2 pos = GetTouchPosition(0);
    auto event = std::make_unique<InputEvent>();
    event->x = pos.x;
    event->y = pos.y;
    if (IsGestureDetected(GESTURE_TAP)) {
        event->inputType = InputEvent::InputType::TAP;
        ViewHandler::getInstance().pushEvent(std::move(event));
    }
}

void DisplayAgent::run() {
    while (_running) {
        handleInput();
        ViewHandler::getInstance().updateView();
        double frameStart = GetTime(); // debugging input delay
        BeginDrawing();
            ClearBackground(BLACK);
            ViewHandler::getInstance().drawView();
        EndDrawing();
        printf("Frame: %.2fms\n", (GetTime() - frameStart) * 1000);
    }
}

