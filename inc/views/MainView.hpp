#pragma once
#include <ViewHandler.hpp>
#include <widgets/Widget.hpp>
#include <vector>

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
        void _drawWidgets();
        std::vector<Widget> _widgets;
};