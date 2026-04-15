#include <agents/OBDAgent.hpp>
#include <chrono>
#include <thread>

OBDAgent::OBDAgent() {

}

OBDAgent::~OBDAgent() {

}

void OBDAgent::start() {
    _running = true;
    _thread = std::jthread([this](std::stop_token st) { run(st); });
}

void OBDAgent::stop() {
    _running = false;
}

void OBDAgent::run(std::stop_token stopToken) {
    while (_running) {


        
    }
}