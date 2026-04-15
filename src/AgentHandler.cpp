#include <AgentHandler.hpp>
#include <optional>

AgentHandler& AgentHandler::getInstance() {
    static AgentHandler instance;
    return instance;
}

void AgentHandler::pushOBDRequest(OBDRequest event) {
    std::lock_guard<std::mutex> lock(_obdMutex);
    _obdQueue.push(event);
}

std::optional<OBDRequest> AgentHandler::popOBDRequest() {
    std::lock_guard<std::mutex> lock(_obdMutex);
    if (_obdQueue.empty()) return std::nullopt;
    OBDRequest event = _obdQueue.front();
    _obdQueue.pop();
    return event;
}

void AgentHandler::pushBluetoothRequest(BTRequest event) {
    std::lock_guard<std::mutex> lock(_btMutex);
    _btQueue.push(event);
}

std::optional<BTRequest> AgentHandler::popBluetoothRequest() {
    std::lock_guard<std::mutex> lock(_btMutex);
    if (_btQueue.empty()) return std::nullopt;
    BTRequest event = _btQueue.front();
    _btQueue.pop();
    return event;
}

void AgentHandler::pushGraphicsRequest(GFXRequest event) {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    _gfxQueue.push(event);
}

std::optional<GFXRequest> AgentHandler::popGFXRequest() {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    if (_gfxQueue.empty()) return std::nullopt;
    GFXRequest event = _gfxQueue.front();
    _gfxQueue.pop();
    return event;
}