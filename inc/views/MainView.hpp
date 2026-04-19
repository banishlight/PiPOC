#pragma once
#include <ViewHandler.hpp>
#include <widgets/Button.hpp>
#include <widgets/Topbar.hpp>
#include <widgets/Bottombar.hpp>
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
        TopBar                               _topBar;
        BottomBar                            _bottomBar; // no main menu button on MainView
};