#include <agents/BluetoothAgent.hpp>
#include <chrono>
#include <thread>

BluetoothAgent::BluetoothAgent() {

}

BluetoothAgent::~BluetoothAgent() {

}

void BluetoothAgent::start() {
    _running = true;
    _thread = std::jthread(&BluetoothAgent::run, this);
}

void BluetoothAgent::stop() {
    _running = false;
}

void BluetoothAgent::run() {
    while (_running) {


        
    }
}