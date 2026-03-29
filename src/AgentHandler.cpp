#include <AgentHandler.hpp>

AgentHandler& AgentHandler::getInstance() {
    static AgentHandler instance;
    return instance;
}

void AgentHandler::pushOBDRequest(OBDEvent event) {
    std::lock_guard<std::mutex> lock(_obdMutex);
    _obdQueue.push(event);
}

OBDEvent AgentHandler::popOBDRequest() {
    std::lock_guard<std::mutex> lock(_obdMutex);
    if (_obdQueue.empty()) return;
    OBDEvent event = _obdQueue.front();
    _obdQueue.pop();
    return event;
}

void AgentHandler::pushBluetoothRequest(BTEvent event) {
    std::lock_guard<std::mutex> lock(_btMutex);
    _btQueue.push(event);
}

BTEvent AgentHandler::popBluetoothRequest() {
    std::lock_guard<std::mutex> lock(_btMutex);
    if (_btQueue.empty()) return;
    BTEvent event = _btQueue.front();
    _btQueue.pop();
    return event;
}

void AgentHandler::pushGraphicsRequest(GFXEvent event) {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    _gfxQueue.push(event);
}

GFXEvent AgentHandler::popGFXRequest() {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    if (_gfxQueue.empty()) return;
    GFXEvent event = _gfxQueue.front();
    _gfxQueue.pop();
    return event;
}