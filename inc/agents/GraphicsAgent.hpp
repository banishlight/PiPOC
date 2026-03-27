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
        std::atomic<bool> _running{false};
};