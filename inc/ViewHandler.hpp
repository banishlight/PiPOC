#pragma once

#include <AgentHandler.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

// Forward declare to avoid circular include — GraphicsAgent includes ViewHandler
class GraphicsAgent;

// ============================================================================
// Notification — passed directly to GraphicsAgent for display
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

        virtual void draw() = 0;
        virtual int logic() = 0;
};

// ============================================================================
// ViewHandler — singleton
// Owns the active view and display event queue.
// ============================================================================
class ViewHandler {
public:
    static ViewHandler& getInstance();

    // Switch to a new view — calls onExit() on old, onEnter() on new
    void switchView(std::unique_ptr<View> view);

    // Called by agents to post events into the display queue
    void pushEvent(std::unique_ptr<AgentEvent> event);

    // Called by current active View
    void processEvents();

    void drawView();

private:
    ViewHandler();
    ~ViewHandler() = default;
    ViewHandler(const ViewHandler&) = delete;
    ViewHandler& operator=(const ViewHandler&) = delete;

    void checkNotification(const AgentEvent& event);

    std::unique_ptr<View> _activeView;
    std::mutex            _viewMutex;

    std::queue<std::unique_ptr<AgentEvent>> _eventQueue;
    std::mutex                              _eventMutex;
};