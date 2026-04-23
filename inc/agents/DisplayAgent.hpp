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
};