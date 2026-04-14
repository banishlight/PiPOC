#include <views/MainView.hpp>
#include <raylib.h>

MainView::MainView() {}

MainView::~MainView() {}

void MainView::start() {

}

void MainView::close() {
    
}

void MainView::draw() {
    DrawText("Main View", 100, 100, 20, WHITE);
}

int MainView::logic() {
    _fetchEvents();

    return 0;
}

void MainView::_fetchEvents() {
    auto events = ViewHandler::getInstance().popViewEvents();
    for (auto& e : events) {
        switch (e->type) {
            case ViewEvent::Type::INPUT: {
                auto* input = static_cast<InputEvent*>(e.get());
                switch (input->inputType) {
                    case InputEvent::InputType::TAP:
                    case InputEvent::InputType::SWIPE_LEFT:
                    case InputEvent::InputType::SWIPE_RIGHT:
                    case InputEvent::InputType::SWIPE_UP:
                    case InputEvent::InputType::SWIPE_DOWN:
                    case InputEvent::InputType::HOLD:
                        for (auto& w : _widgets) {
                            if (w.handleEvent(*input)) break;
                        }
                        break;
                    default:
                        // Pass event onto notifcation handler? Later.
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
}