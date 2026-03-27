#include <agents/GraphicsAgent.hpp>
#include <ViewHandler.hpp>
#include <chrono>
#include <thread>
#include <raylib.h>

GraphicsAgent::GraphicsAgent() {
    InitWindow(1024, 600, "PiPOC");
}

GraphicsAgent::~GraphicsAgent() {

}

void GraphicsAgent::start() {
    _running = true;
    run();
}

void GraphicsAgent::stop() {
    _running = false;
}

void GraphicsAgent::run() {
    while (_running) {
        ViewHandler::getInstance().updateView();
        BeginDrawing();
        ViewHandler::getInstance().drawView();
        drawNotif();
        EndDrawing();
    }
}

// Draw any current notifications ontop of current view
void GraphicsAgent::drawNotif() {

}