#include <AgentHandler.hpp>
#include <optional>

AgentHandler& AgentHandler::getInstance() {
    static AgentHandler instance;
    return instance;
}

void AgentHandler::pushOBDRequest(OBDEvent event) {
    std::lock_guard<std::mutex> lock(_obdMutex);
    _obdQueue.push(event);
}

std::optional<OBDEvent> AgentHandler::popOBDRequest() {
    std::lock_guard<std::mutex> lock(_obdMutex);
    if (_obdQueue.empty()) return std::nullopt;
    OBDEvent event = _obdQueue.front();
    _obdQueue.pop();
    return event;
}

void AgentHandler::pushBluetoothRequest(BTEvent event) {
    std::lock_guard<std::mutex> lock(_btMutex);
    _btQueue.push(event);
}

std::optional<BTEvent> AgentHandler::popBluetoothRequest() {
    std::lock_guard<std::mutex> lock(_btMutex);
    if (_btQueue.empty()) return std::nullopt;
    BTEvent event = _btQueue.front();
    _btQueue.pop();
    return event;
}

void AgentHandler::pushGraphicsRequest(GFXEvent event) {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    _gfxQueue.push(event);
}

std::optional<GFXEvent> AgentHandler::popGFXRequest() {
    std::lock_guard<std::mutex> lock(_gfxMutex);
    if (_gfxQueue.empty()) return std::nullopt;
    GFXEvent event = _gfxQueue.front();
    _gfxQueue.pop();
    return event;
}