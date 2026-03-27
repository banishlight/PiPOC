#include <agents/InputAgent.hpp>
#include <chrono>
#include <thread>

InputAgent::InputAgent() {

}

InputAgent::~InputAgent() {

}

void InputAgent::start() {
    _running = true;
    _thread = std::jthread(&InputAgent::run, this);
}

void InputAgent::stop() {
    _running = false;
}

void InputAgent::run() {
    while (_running) {


        
    }
}