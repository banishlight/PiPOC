#pragma once
#include <atomic>
#include <thread>

class OBDAgent {
    public:
        OBDAgent();
        ~OBDAgent();
        void start();
        void stop();
    private:
        void run();
        std::jthread _thread;
        std::atomic<bool> _running{false};
};