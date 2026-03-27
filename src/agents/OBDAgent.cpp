#include <agents/OBDAgent.hpp>
#include <chrono>
#include <jthread>

GraphicsAgent::OBDAgent() {

}

GraphicsAgent::~OBDAgent() {

}

void OBDAgent::start() {
    _running = true;
    _thread = std::jthread(&GraphicsAgent::run, this);
}

void OBDAgent::stop() {
    _running = false;
}

void OBDAgent::run() {
    while (_running) {


        
    }
}