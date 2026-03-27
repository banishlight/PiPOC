#include <views/MainView.hpp>
#include <raylib.h>

MainView::MainView() {}

MainView::~MainView() {}

void MainView::draw() {
    DrawText("Main View", 100, 100, 20, WHITE);
}

int MainView::logic() {


    return 0;
}

void MainView::fetchEvents() {

}