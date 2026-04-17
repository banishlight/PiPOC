#pragma once
#include <thread>
#include <SerialPort.hpp>
 
class OBDAgent {
    public:
        OBDAgent();
        ~OBDAgent();
 
        // Not copyable or movable — owns a thread and serial port
        OBDAgent(const OBDAgent&)            = delete;
        OBDAgent& operator=(const OBDAgent&) = delete;
 
        void start();
        void stop();
 
    private:
        void run(std::stop_token stopToken);
 
        // Sends a command and returns the raw response string.
        // Returns empty string on failure.
        std::string sendCommand(const std::string& cmd);
 
        // ELM327 initialisation sequence.
        // Returns true if the adapter acknowledged correctly.
        bool initELM327();
 
        // PID polling — each returns the decoded value or -1.0f on failure.
        float pollRPM();
        float pollCoolantTemp();
        float pollSpeed();
        float pollThrottlePos();
 
        std::jthread _thread;
        SerialPort   _serial;
};