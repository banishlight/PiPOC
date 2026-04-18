#pragma once
#include <ViewHandler.hpp>
#include <widgets/Button.hpp>
#include <vector>
#include <memory>

class MainView : public View {
    public:
        MainView();
        ~MainView();
        void draw() override;
        int logic() override;
        void start() override;
        void close() override;
    private:
        void _fetchEvents();
        void _initButtons();

        std::vector<std::unique_ptr<Button>> _navButtons;
        std::unique_ptr<Button>              _settingsButton;
};