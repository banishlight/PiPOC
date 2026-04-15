#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <optional>

class OBDRequest {

};

class BTRequest {

};

class GFXRequest {

};

// AgentHandler — singleton owning per-agent request queues
// Views post requests here; each agent reads from its own queue.
class AgentHandler {
public:
    static AgentHandler& getInstance();

    void pushOBDRequest(OBDRequest event);
    std::optional<OBDRequest> popOBDRequest();

    void pushBluetoothRequest(BTRequest event);
    std::optional<BTRequest> popBluetoothRequest();

    void pushGraphicsRequest(GFXRequest event);
    std::optional<GFXRequest> popGFXRequest();

private:
    AgentHandler() = default;
    ~AgentHandler() = default;
    AgentHandler(const AgentHandler&) = delete;
    AgentHandler& operator=(const AgentHandler&) = delete;

    std::queue<OBDRequest> _obdQueue;
    std::mutex           _obdMutex;

    std::queue<BTRequest>  _btQueue;
    std::mutex           _btMutex;

    std::queue<GFXRequest> _gfxQueue;
    std::mutex           _gfxMutex;
};