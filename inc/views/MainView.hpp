#pragma once
#include <ViewHandler.hpp>

class MainView : public View {
    public:
        MainView();
        ~MainView();
        void draw() override;
        int logic() override;
        
    private:
        void fetchEvents();
};