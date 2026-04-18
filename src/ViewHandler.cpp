#include <ViewHandler.hpp>
#include <agents/DisplayAgent.hpp>
#include <views/MainView.hpp>
// #include <views/OBDView.hpp>

ViewHandler::ViewHandler() {
    // Startup view
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

void ViewHandler::pushEvent(std::unique_ptr<ViewEvent> event) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventQueue.push_back(std::move(event));
}

std::vector<std::unique_ptr<ViewEvent>> ViewHandler::popViewEvents() {
    std::lock_guard<std::mutex> lock(_eventMutex);
    std::vector<std::unique_ptr<ViewEvent>> e = std::move(_eventQueue);
    _eventQueue.clear();
    return e;
}

void ViewHandler::updateView() {
    if (!_activeView) return;
    _activeView->logic();
}

void ViewHandler::drawView() {
    if (!_activeView) return;
    _activeView->draw();
}

void ViewHandler::setConnectedDevice(std::string name) {
    _isBTConnect = true;
    _deviceName = name;
}

const std::string& ViewHandler::getConnectedDevice() const {
    return _deviceName;
}

bool ViewHandler::isDeviceConnected() const {
    return _isBTConnect;
}

void ViewHandler::clearConnectedDevice() {
    _deviceName = "Not Connected";
    _isBTConnect = false;
}