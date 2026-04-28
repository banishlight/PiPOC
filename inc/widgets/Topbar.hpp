#pragma once
#include <widgets/Widget.hpp>

// TopBar — shared across all views
// Displays: car model, ECU status, WiFi status, clock, CPU temp, CPU load
// Read from ViewHandler at draw time — no state needed

class TopBar : public Widget {
public:
    TopBar();

    void draw() override;
    bool handleEvent(const InputEvent&) override { return false; }

    static constexpr int HEIGHT = 48;
};