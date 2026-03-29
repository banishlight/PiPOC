#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <string>

class OBDEvent {

};

class BTEvent {

};

class GFXEvent {

};

// AgentHandler — singleton owning per-agent request queues
// Views post requests here; each agent reads from its own queue.
class AgentHandler {
public:
    static AgentHandler& getInstance();

    void pushOBDRequest(OBDEvent event);
    OBDEvent popOBDRequest();

    void pushBluetoothRequest(BTEvent event);
    BTEvent popBluetoothRequest();

    void pushGraphicsRequest(GFXEvent event);
    GFXEvent popGFXRequest();

private:
    AgentHandler() = default;
    ~AgentHandler() = default;
    AgentHandler(const AgentHandler&) = delete;
    AgentHandler& operator=(const AgentHandler&) = delete;

    std::queue<OBDEvent> _obdQueue;
    std::mutex           _obdMutex;

    std::queue<BTEvent>  _btQueue;
    std::mutex           _btMutex;

    std::queue<GFXEvent> _gfxQueue;
    std::mutex           _gfxMutex;
};