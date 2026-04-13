#pragma once
#include <atomic>
#include <thread>

class DisplayAgent {
    public:
        DisplayAgent();
        ~DisplayAgent();
        void start();
        void stop();
    private:
        void run();
        void handleInput();
        std::atomic<bool> _running{false};

        int _holdTime = 0;
        static constexpr int HOLD_THRESH  = 20;
        static constexpr int SWIPE_THRESH = 50;
};