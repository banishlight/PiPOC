#pragma once
#include <atomic>
#include <thread>

class GraphicsAgent {
    public:
        GraphicsAgent();
        ~GraphicsAgent();
        void start();
        void stop();
    private:
        void run();
        void drawNotif();
        std::jthread _thread;
        std::atomic<bool> _running{false};
};