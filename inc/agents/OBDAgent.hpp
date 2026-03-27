#pragma once
#include <atomic>
#include <jthread>

class OBDAgent {
    public:
        GraphicsAgent();
        ~GraphicsAgent();
        void start();
        void stop();
    private:
        void run();
        std::jthread _thread;
        std::atomic<bool> _running{false};
};