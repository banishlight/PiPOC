#pragma once
#include <atomic>
#include <thread>

class BluetoothAgent {
    public:
        BluetoothAgent();
        ~BluetoothAgent();
        void start();
        void stop();
    private:
        void run();
        std::jthread _thread;
        std::atomic<bool> _running{false};
};