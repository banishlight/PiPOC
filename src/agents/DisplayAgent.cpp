#include <agents/DisplayAgent.hpp>
#include <ViewHandler.hpp>
#include <chrono>
#include <thread>
#include <raylib.h>

DisplayAgent::DisplayAgent() {
    InitWindow(1024, 600, "PiPOC");
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
            InputEvent event;
            event.inputType = InputEvent::InputType::HOLD;
            event.x = pos.x;
            event.y = pos.y;
            ViewHandler::getInstance().pushEvent(event);
        }
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        _holdTime = 0;
        // release frame
        Vector2 delta = GetMouseDelta();
        InputEvent event;
        event.x = delta.x;
        event.y = delta.y;
        if (delta.x > SWIPE_THRESH)       event.inputType = InputEvent::InputType::SWIPE_RIGHT;
        else if (delta.x < -SWIPE_THRESH) event.inputType = InputEvent::InputType::SWIPE_LEFT;
        else if (delta.y > SWIPE_THRESH)  event.inputType = InputEvent::InputType::SWIPE_DOWN;
        else if (delta.y < -SWIPE_THRESH) event.inputType = InputEvent::InputType::SWIPE_UP;
        else { 
            // Didn't move far enough or hold long enough, TAP
            Vector2 pos = GetMousePosition();
            event.x = pos.x;
            event.y = pos.y;
            event.inputType = InputEvent::InputType::TAP;
        }
        ViewHandler::getInstance().pushEvent(event);
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

