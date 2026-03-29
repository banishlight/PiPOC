#include <ViewHandler.hpp>
#include <agents/DisplayAgent.hpp>
#include <views/MainView.hpp>
// #include <views/OBDView.hpp>
#include <optional>

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

void ViewHandler::pushEvent(ViewEvent event) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventQueue.push(event);
}

std::optional<ViewEvent> ViewHandler::popViewEvent() {
    std::lock_guard<std::mutex> lock(_eventMutex);
    if (_eventQueue.empty()) return std::nullopt;
    ViewEvent event = _eventQueue.front();
    _eventQueue.pop();
    return event;
}

void ViewHandler::updateView() {
    if (!_activeView) return;
    _activeView->logic();
}

void ViewHandler::drawView() {
    if (!_activeView) return;
    _activeView->draw();
}