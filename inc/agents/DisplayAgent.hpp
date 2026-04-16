#pragma once
#include <atomic>
#include <thread>
#include <raylib.h>

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

        static constexpr int MAX_FPS = 60;
        static constexpr int DIS_WIDTH = 1024;
        static constexpr int DIS_HEIGHT = 600;
};