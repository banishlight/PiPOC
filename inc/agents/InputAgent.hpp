#pragma once
#include <atomic>
#include <thread>

class InputAgent {
    public:
        InputAgent();
        ~InputAgent();
        void start();
        void stop();
    private:
        void run();
        std::jthread _thread;
        std::atomic<bool> _running{false};
};