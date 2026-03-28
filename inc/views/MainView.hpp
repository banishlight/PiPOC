#pragma once
#include <ViewHandler.hpp>

class MainView : public View {
    public:
        MainView();
        ~MainView();
        void draw() override;
        int logic() override;
        void start() override;
        void close() override;
    private:
        void fetchEvents();
};