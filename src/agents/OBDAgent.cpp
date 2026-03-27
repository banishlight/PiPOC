#include <agents/OBDAgent.hpp>
#include <chrono>
#include <thread>

OBDAgent::OBDAgent() {

}

OBDAgent::~OBDAgent() {

}

void OBDAgent::start() {
    _running = true;
    _thread = std::jthread(&OBDAgent::run, this);
}

void OBDAgent::stop() {
    _running = false;
}

void OBDAgent::run() {
    while (_running) {


        
    }
}