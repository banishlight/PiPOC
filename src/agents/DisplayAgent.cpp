#include <agents/DisplayAgent.hpp>
#include <ViewHandler.hpp>
#include <Assets.hpp>
#include <chrono>
#include <thread>
#include <raylib.h>
#include <memory>

DisplayAgent::DisplayAgent() {
    InitWindow(DIS_WIDTH, DIS_HEIGHT, "PiPOC");
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
    // Simple, only taps for now
    // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    //     Vector2 pos = GetMousePosition();
    //     auto event = std::make_unique<InputEvent>();
    //     event->inputType = InputEvent::InputType::TAP;
    //     event->x = pos.x;
    //     event->y = pos.y;
    //     ViewHandler::getInstance().pushEvent(std::move(event));
    // }
    Vector2 pos = GetMousePosition();
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
        BeginDrawing();
            ClearBackground(BLACK);
            ViewHandler::getInstance().drawView();
        EndDrawing();
    }
}

