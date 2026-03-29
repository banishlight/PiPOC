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
        void drawNotif();
        std::atomic<bool> _running{false};
};