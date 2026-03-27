#include <agents/GraphicsAgent.hpp>
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
        drawView();
        drawNotif();
    }
}

void GraphicsAgent::drawView() {

}

void GraphicsAgent::drawNotif() {

}