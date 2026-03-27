#include <AgentHandler.hpp>

AgentHandler& AgentHandler::getInstance() {
    static AgentHandler instance;
    return instance;
}

void AgentHandler::pushOBDRequest(std::unique_ptr<AgentEvent> event) {
    std::lock_guard<std::mutex> lock(_obdMutex);
    _obdQueue.push(std::move(event));
}

std::unique_ptr<AgentEvent> AgentHandler::popOBDRequest() {
    std::lock_guard<std::mutex> lock(_obdMutex);
    if (_obdQueue.empty()) return nullptr;
    auto event = std::move(_obdQueue.front());
    _obdQueue.pop();
    return event;
}

void AgentHandler::pushBluetoothRequest(std::unique_ptr<AgentEvent> event) {
    std::lock_guard<std::mutex> lock(_btMutex);
    _btQueue.push(std::move(event));
}

std::unique_ptr<AgentEvent> AgentHandler::popBluetoothRequest() {
    std::lock_guard<std::mutex> lock(_btMutex);
    if (_btQueue.empty()) return nullptr;
    auto event = std::move(_btQueue.front());
    _btQueue.pop();
    return event;
}