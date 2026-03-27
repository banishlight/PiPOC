#include <ViewHandler.hpp>
#include <agents/GraphicsAgent.hpp>

ViewHandler& ViewHandler::getInstance() {
    static ViewHandler instance;
    return instance;
}

void ViewHandler::switchView(std::unique_ptr<View> view) {
    std::lock_guard<std::mutex> lock(_viewMutex);
    if (_activeView) _activeView->onExit();
    _activeView = std::move(view);
    if (_activeView) _activeView->onEnter();
}

void ViewHandler::pushEvent(std::unique_ptr<AgentEvent> event) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventQueue.push(std::move(event));
}

void ViewHandler::processEvents() {

}

void ViewHandler::checkNotification(const AgentEvent& event) {

}