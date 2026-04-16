#pragma once
#include <ViewHandler.hpp>
#include <widgets/Widget.hpp>
#include <vector>
#include <functional>

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

        bool                  _transitioning    = false;
        float                 _transitionTimer  = 0.0f;
        int                   _hoveredBtn       = -1;
        std::function<void()> _pendingSwitch;
    
        static constexpr float kTransitionDuration = 0.15f;
};