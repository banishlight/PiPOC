#include <views/MainView.hpp>
#include <raylib.h>

MainView::MainView() {}

MainView::~MainView() {}

void MainView::onEnter() {}

void MainView::onExit() {}

void MainView::update(float dt) {}

void MainView::draw() {
    DrawText("Main View", 100, 100, 20, WHITE);
}

bool MainView::handleEvent(const AgentEvent& event) {
    return false;
}