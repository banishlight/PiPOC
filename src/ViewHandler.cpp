#include "ViewHandler.hpp"

ViewHandler& ViewHandler::getInstance() {
    static ViewHandler instance;
    return instance;
}

void ViewHandler::pushEvent(std::unique_ptr<AgentEvent> event) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventQueue.push(std::move(event));
}

void ViewHandler::processEvents(View* activeView) {
    if (!activeView) return;

    std::unique_ptr<AgentEvent> event;
    {
        std::lock_guard<std::mutex> lock(_eventMutex);
        if (_eventQueue.empty()) return;
        event = std::move(_eventQueue.front());
        _eventQueue.pop();
    }

    bool consumed = activeView->handleEvent(*event);
    if (!consumed) {
        checkNotification(*event);
    }
}

void ViewHandler::pushNotification(Notification notif) {
    std::lock_guard<std::mutex> lock(_notifMutex);
    _notifQueue.push(std::move(notif));
}

std::unique_ptr<Notification> ViewHandler::popNotification() {
    std::lock_guard<std::mutex> lock(_notifMutex);
    if (_notifQueue.empty()) return nullptr;
    auto notif = std::make_unique<Notification>(std::move(_notifQueue.front()));
    _notifQueue.pop();
    return notif;
}

void ViewHandler::checkNotification(const AgentEvent& event) {
    if (event.type != AgentEvent::Type::OBD) return;

    const auto& obd = static_cast<const OBDEvent&>(event);

    if (obd.coolantTemp > kCoolantWarnTemp) {
        pushNotification({"Coolant temp high: "
            + std::to_string((int)obd.coolantTemp) + " C", 4.0f});
    }
    if (obd.oilTemp > kOilWarnTemp) {
        pushNotification({"Oil temp high: "
            + std::to_string((int)obd.oilTemp) + " C", 4.0f});
    }
}