#include <views/MainView.hpp>
#include <views/OBDView.hpp>
#include <views/MusicView.hpp>
#include <widgets/Button.hpp>
#include <raylib.h>

MainView::MainView() {}

MainView::~MainView() {}

void MainView::start() {
    auto obdBtn = std::make_unique<Button>(112, 200, 200, 60, "OBD");
    obdBtn->setOnClick([this]() {
        ViewHandler::getInstance().switchView(std::make_unique<OBDView>());
    });
    _widgets.push_back(std::move(obdBtn));

    auto mscBtn = std::make_unique<Button>(412, 200, 200, 60, "Music");
    mscBtn->setOnClick([this]() {
        ViewHandler::getInstance().switchView(std::make_unique<MusicView>());
    });
    _widgets.push_back(std::move(mscBtn));

    // Un comment when map is implemented
    // auto mapBtn = std::make_unique<Button>(412, 200, 200, 60, "Map");
    // mapBtn->setOnClick([this]() {
    //     ViewHandler::getInstance().switchView(std::make_unique<MapView>());
    // });
    // _widgets.push_back(std::move(mapBtn));
}

void MainView::close() {
    
}

void MainView::draw() {
    for (auto& widget : _widgets) {
        widget->draw();
    }
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
                        for (auto& w : _widgets) {
                            if (w->handleEvent(*input)) break;
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