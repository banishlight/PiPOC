#pragma once
#include <widgets/Widget.hpp>
#include <widgets/Button.hpp>
#include <memory>

// BottomBar — shared across all views
// Always shows: BT device name, settings button, software version
// Optionally shows: main menu button (pass showMainMenu = true)

class BottomBar : public Widget {
public:
    explicit BottomBar(bool showMainMenu = false);

    void draw() override;
    bool handleEvent(const InputEvent& event) override;

    static constexpr int HEIGHT = 48;

private:
    std::unique_ptr<Button> _settingsButton;
    std::unique_ptr<Button> _mainMenuButton;
    bool                    _showMainMenu;
};