#pragma once
#include <thread>
#include <cstdint>

class SystemAgent {
public:
    SystemAgent();
    ~SystemAgent();

    SystemAgent(const SystemAgent&)            = delete;
    SystemAgent& operator=(const SystemAgent&) = delete;

    void start();
    void stop();

private:
    void run(std::stop_token stopToken);

    float readCPUTemp() const;
    float readCPULoad();
    bool  readWifiStatus() const;

    std::jthread _thread;

    // CPU load calculation requires two samples to compute a delta
    uint64_t _prevIdle    = 0;
    uint64_t _prevTotal   = 0;

    static constexpr int    POLL_INTERVAL_MS  = 1000;
    static constexpr const char* WIFI_IFACE   = "wlan0";
};