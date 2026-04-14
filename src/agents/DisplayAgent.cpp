#include <agents/DisplayAgent.hpp>
#include <ViewHandler.hpp>
#include <chrono>
#include <thread>
#include <raylib.h>
#include <memory>

DisplayAgent::DisplayAgent() {
    InitWindow(DIS_WIDTH, DIS_HEIGHT, "PiPOC");
    SetTargetFPS(MAX_FPS);
}

DisplayAgent::~DisplayAgent() {

}

void DisplayAgent::start() {
    _running = true;
    run();
}

void DisplayAgent::stop() {
    _running = false;
}

void DisplayAgent::handleInput() {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        _holdTime += 1;
        if (_holdTime > HOLD_THRESH){
            Vector2 pos = GetMousePosition();
            auto event = std::make_unique<InputEvent>();
            event->inputType = InputEvent::InputType::HOLD;
            event->x = pos.x;
            event->y = pos.y;
            ViewHandler::getInstance().pushEvent(std::move(event));
        }
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        _holdTime = 0;
        // release frame
        Vector2 delta = GetMouseDelta();
        auto event = std::make_unique<InputEvent>();
        event->x = delta.x;
        event->y = delta.y;
        if (delta.x > SWIPE_THRESH)       event->inputType = InputEvent::InputType::SWIPE_RIGHT;
        else if (delta.x < -SWIPE_THRESH) event->inputType = InputEvent::InputType::SWIPE_LEFT;
        else if (delta.y > SWIPE_THRESH)  event->inputType = InputEvent::InputType::SWIPE_DOWN;
        else if (delta.y < -SWIPE_THRESH) event->inputType = InputEvent::InputType::SWIPE_UP;
        else { 
            // Didn't move far enough or hold long enough, TAP
            Vector2 pos = GetMousePosition();
            event->x = pos.x;
            event->y = pos.y;
            event->inputType = InputEvent::InputType::TAP;
        }
        ViewHandler::getInstance().pushEvent(std::move(event));
    }
}

void DisplayAgent::run() {
    while (_running) {
        handleInput();
        ViewHandler::getInstance().updateView();
        BeginDrawing();
        ViewHandler::getInstance().drawView();
        EndDrawing();
    }
}

