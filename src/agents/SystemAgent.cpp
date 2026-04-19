#include <agents/SystemAgent.hpp>
#include <ViewHandler.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

SystemAgent::SystemAgent() {}

SystemAgent::~SystemAgent() {
    stop();
}

void SystemAgent::start() {
    _thread = std::jthread([this](std::stop_token st) { run(st); });
}

void SystemAgent::stop() {
    _thread.request_stop();
    if (_thread.joinable())
        _thread.join();
}

void SystemAgent::run(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        ViewHandler::getInstance().setCPUTemp(readCPUTemp());
        ViewHandler::getInstance().setCPULoad(readCPULoad());
        ViewHandler::getInstance().setWifiConnected(readWifiStatus());

        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_INTERVAL_MS));
    }
}

float SystemAgent::readCPUTemp() const {
    std::ifstream file("/sys/class/thermal/thermal_zone0/temp");
    if (!file.is_open()) {
        std::cerr << "[SYS] Could not read CPU temp\n";
        return 0.0f;
    }

    int millidegrees = 0;
    file >> millidegrees;
    return millidegrees / 1000.0f;
}

float SystemAgent::readCPULoad() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        std::cerr << "[SYS] Could not read /proc/stat\n";
        return 0.0f;
    }

    // First line: "cpu user nice system idle iowait irq softirq steal guest guest_nice"
    std::string label;
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
    file >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    uint64_t idleTime  = idle + iowait;
    uint64_t totalTime = user + nice + system + idle + iowait + irq + softirq + steal;

    uint64_t deltaIdle  = idleTime  - _prevIdle;
    uint64_t deltaTotal = totalTime - _prevTotal;

    _prevIdle  = idleTime;
    _prevTotal = totalTime;

    if (deltaTotal == 0) return 0.0f;

    float load = 100.0f * (1.0f - (float)deltaIdle / (float)deltaTotal);
    return load < 0.0f ? 0.0f : load;
}

bool SystemAgent::readWifiStatus() const {
    std::ifstream file("/proc/net/wireless");
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.find(WIFI_IFACE) != std::string::npos)
            return true;
    }

    return false;
}