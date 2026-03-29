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

void DisplayAgent::run() {
    while (_running) {
        ViewHandler::getInstance().updateView();
        BeginDrawing();
        ViewHandler::getInstance().drawView();
        drawNotif();
        EndDrawing();
    }
}

// Draw any current notifications ontop of current view
void DisplayAgent::drawNotif() {

}