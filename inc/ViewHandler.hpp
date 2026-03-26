#pragma once

#include "AgentHandler.hpp"
#include <memory>
#include <mutex>
#include <queue>
#include <string>

// ============================================================================
// Notification — message overlaid on screen by GraphicsAgent
// ============================================================================
struct Notification {
    std::string message;
    float       duration = 3.0f;   // seconds to display
};

// ============================================================================
// View — abstract base class for all views
// ============================================================================
class View {
public:
    virtual ~View() = default;

    virtual void onEnter() {}
    virtual void onExit()  {}

    virtual void update(float dt) = 0;
    virtual void draw()           = 0;

    // Inspect the front event. Return true to consume it, false to discard.
    virtual bool handleEvent(const AgentEvent& event) = 0;
};

// ============================================================================
// ViewHandler — singleton
// Owns the display event queue and notification queue.
// ============================================================================
class ViewHandler {
public:
    static ViewHandler& getInstance();

    // Called by agents to post events
    void pushEvent(std::unique_ptr<AgentEvent> event);

    // Called by GraphicsAgent each frame — offers front event to active view,
    // routes discards through notification logic
    void processEvents(View* activeView);

    // Called by GraphicsAgent to retrieve pending notification overlays
    std::unique_ptr<Notification> popNotification();

private:
    ViewHandler() = default;
    ~ViewHandler() = default;
    ViewHandler(const ViewHandler&) = delete;
    ViewHandler& operator=(const ViewHandler&) = delete;

    void pushNotification(Notification notif);
    void checkNotification(const AgentEvent& event);

    static constexpr float kCoolantWarnTemp = 100.0f;
    static constexpr float kOilWarnTemp     = 130.0f;

    std::queue<std::unique_ptr<AgentEvent>> _eventQueue;
    std::mutex                              _eventMutex;

    std::queue<Notification> _notifQueue;
    std::mutex               _notifMutex;
};