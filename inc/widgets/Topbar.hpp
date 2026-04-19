#pragma once
#include <widgets/Widget.hpp>

// TopBar — shared across all views
// Displays: car model, ECU status, WiFi status, clock, CPU temp, CPU load
// Read from ViewHandler at draw time — no state needed

class TopBar : public Widget {
public:
    TopBar();

    void draw() override;
    bool handleEvent(const InputEvent& event) override { return false; }

    static constexpr int HEIGHT = 32;
    static constexpr char* CAR_NAME = "MAZDA RX-8  13B-MSP";
};