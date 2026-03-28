#pragma once

#include <AgentHandler.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

// Notification — passed directly to GraphicsAgent for display
struct Notification {
    std::string message;
    float       duration = 3.0f;   // seconds to display
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
        void pushEvent(std::unique_ptr<AgentEvent> event);

        // Called by current active View
        void processEvents();
        void updateView();
        void drawView();

    private:
        ViewHandler();
        ~ViewHandler();
        ViewHandler(const ViewHandler&) = delete;
        ViewHandler& operator=(const ViewHandler&) = delete;

        std::unique_ptr<View> _activeView;
        std::mutex            _viewMutex;

        std::queue<std::unique_ptr<AgentEvent>> _eventQueue;
        std::mutex                              _eventMutex;
};