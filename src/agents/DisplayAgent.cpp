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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _pressPos = GetMousePosition();
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        _holdTime += 1;
        if (_holdTime > HOLD_THRESH) {
            Vector2 pos = GetMousePosition();
            auto event = std::make_unique<InputEvent>();
            event->inputType = InputEvent::InputType::HOLD;
            event->x = pos.x;
            event->y = pos.y;
            ViewHandler::getInstance().pushEvent(std::move(event));
        }
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        _holdTime = 0;
        Vector2 releasePos = GetMousePosition();
        float dx = releasePos.x - _pressPos.x;
        float dy = releasePos.y - _pressPos.y;

        auto event = std::make_unique<InputEvent>();
        if (dx > SWIPE_THRESH)       event->inputType = InputEvent::InputType::SWIPE_RIGHT;
        else if (dx < -SWIPE_THRESH) event->inputType = InputEvent::InputType::SWIPE_LEFT;
        else if (dy > SWIPE_THRESH)  event->inputType = InputEvent::InputType::SWIPE_DOWN;
        else if (dy < -SWIPE_THRESH) event->inputType = InputEvent::InputType::SWIPE_UP;
        else {
            event->inputType = InputEvent::InputType::TAP;
            event->x = releasePos.x;
            event->y = releasePos.y;
        }
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

