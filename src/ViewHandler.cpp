#include <ViewHandler.hpp>
#include <agents/GraphicsAgent.hpp>
#include <views/MainView.hpp>
// #include <views/OBDView.hpp>

ViewHandler::ViewHandler() {
    _activeView = std::make_unique<MainView>();
    _activeView->start();
}

ViewHandler::~ViewHandler() {
    _activeView->close();
}

ViewHandler& ViewHandler::getInstance() {
    static ViewHandler instance;
    return instance;
}

void ViewHandler::switchView(std::unique_ptr<View> view) {
    std::lock_guard<std::mutex> lock(_viewMutex);
    if (_activeView) _activeView->close();
    _activeView = std::move(view);
    if (_activeView) _activeView->start();
}

void ViewHandler::pushEvent(std::unique_ptr<AgentEvent> event) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventQueue.push(std::move(event));
}

void ViewHandler::processEvents() {

}

void ViewHandler::updateView() {
    if (!_activeView) return;
    _activeView->logic();
}

void ViewHandler::drawView() {
    if (!_activeView) return;
    _activeView->draw();
}