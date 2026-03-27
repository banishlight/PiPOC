#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <string>

// AgentEvent — base class for all agent events
struct AgentEvent {
    enum class Type {
        OBD,
        Bluetooth,
        Input,
    };

    const Type type;
    explicit AgentEvent(Type t) : type(t) {}
    virtual ~AgentEvent() = default;
};

// OBDEvent — emitted by OBDAgent
struct OBDEvent : public AgentEvent {
    float rpm          = 0.0f;
    float coolantTemp  = 0.0f;
    float oilTemp      = 0.0f;
    float throttle     = 0.0f;
    float vehicleSpeed = 0.0f;
    float intakeTemp   = 0.0f;
    float maf          = 0.0f;
    bool  connected    = false;

    OBDEvent() : AgentEvent(Type::OBD) {}
};

// BluetoothEvent — emitted by BluetoothAgent
struct BluetoothEvent : public AgentEvent {
    enum class BTType {
        TrackChanged,
        PlaybackStateChanged,
        DeviceConnected,
        DeviceDisconnected,
        VolumeChanged,
    };

    BTType      btType = BTType::TrackChanged;
    std::string title;
    std::string artist;
    std::string deviceName;
    bool        playing = false;
    int         volume  = 0;    // 0–100

    BluetoothEvent() : AgentEvent(Type::Bluetooth) {}
};

// InputEvent — emitted by InputAgent
struct InputEvent : public AgentEvent {
    enum class InputType {
        TouchPress,
        TouchRelease,
        TouchDrag,
        Gesture,
    };

    enum class Gesture {
        None,
        SwipeLeft,
        SwipeRight,
        SwipeUp,
        SwipeDown,
        Tap,
        LongPress,
    };

    InputType inputType = InputType::TouchPress;
    Gesture   gesture   = Gesture::None;
    float     x         = 0.0f;
    float     y         = 0.0f;

    InputEvent() : AgentEvent(Type::Input) {}
};

// AgentHandler — singleton owning per-agent request queues
// Views post requests here; each agent reads from its own queue.
class AgentHandler {
public:
    static AgentHandler& getInstance();

    void pushOBDRequest(std::unique_ptr<AgentEvent> event);
    std::unique_ptr<AgentEvent> popOBDRequest();

    void pushBluetoothRequest(std::unique_ptr<AgentEvent> event);
    std::unique_ptr<AgentEvent> popBluetoothRequest();

private:
    AgentHandler() = default;
    ~AgentHandler() = default;
    AgentHandler(const AgentHandler&) = delete;
    AgentHandler& operator=(const AgentHandler&) = delete;

    std::queue<std::unique_ptr<AgentEvent>> _obdQueue;
    std::mutex                              _obdMutex;

    std::queue<std::unique_ptr<AgentEvent>> _btQueue;
    std::mutex                              _btMutex;
};