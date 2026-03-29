#pragma once

#include <AgentHandler.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <optional>

// Notification — passed directly to GraphicsAgent for display
struct Notification {
    std::string message;
    float       duration = 3.0f;   // seconds to display
};

// Events to be processed by the view
class ViewEvent {
    enum type {
        TAP,
        HOLD,
        
    };
};

// View — abstract base class for all views
class View {
    public:
        virtual ~View() = default;

        virtual void draw() = 0;
        virtual int logic() = 0;
        virtual void start() = 0;
        virtual void close() = 0;
};

// ViewHandler — singleton
// Owns the active view and display event queue.
class ViewHandler {
    public:
        static ViewHandler& getInstance();
        void switchView(std::unique_ptr<View> view);
        void pushEvent(ViewEvent event);
        std::optional<ViewEvent> popViewEvent();

        // Called by current active View
        void updateView();
        void drawView();

    private:
        ViewHandler();
        ~ViewHandler();
        ViewHandler(const ViewHandler&) = delete;
        ViewHandler& operator=(const ViewHandler&) = delete;

        std::unique_ptr<View> _activeView;
        std::mutex            _viewMutex;

        std::queue<ViewEvent> _eventQueue;
        std::mutex            _eventMutex;
};