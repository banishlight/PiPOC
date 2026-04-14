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
    public:
        enum class Type {
            INPUT,
            OBD,
            BLUETOOTH,
        };
        const Type type;
        explicit ViewEvent(Type t) : type(t) {}
        virtual ~ViewEvent() = default;
};

class InputEvent : public ViewEvent {
    public:
        InputEvent() : ViewEvent(Type::INPUT) {}
        enum class InputType {
            TAP,
            HOLD,
            SWIPE_LEFT,
            SWIPE_RIGHT,
            SWIPE_UP,
            SWIPE_DOWN
        };
        InputType inputType = InputType::TAP;
        float x = 0.0f;
        float y = 0.0f;
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
        void pushEvent(std::unique_ptr<ViewEvent> event);
        std::vector<std::unique_ptr<ViewEvent>> popViewEvents();

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

        std::vector<std::unique_ptr<ViewEvent>> _eventQueue;
        std::mutex            _eventMutex;
};