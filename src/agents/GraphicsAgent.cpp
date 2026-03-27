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
    _thread = std::jthread(&GraphicsAgent::run, this);
}

void GraphicsAgent::stop() {
    _running = false;
}

void GraphicsAgent::run() {
    while (_running) {
        BeginDrawing();
        ViewHandler::getInstance().drawView();
        drawNotif();
        EndDrawing();
    }
}

void GraphicsAgent::drawNotif() {

}