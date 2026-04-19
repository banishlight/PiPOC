#include <agents/OBDAgent.hpp>
#include <ViewHandler.hpp>
#include <Config.hpp>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <iomanip>

OBDAgent::OBDAgent()
    : _serial(SerialConfig{
        .device    = OBD_SERIAL_DEVICE,
        .baudRate  = OBD_BAUD_RATE,
        .dataBits  = 8,
        .stopBits  = 1,
        .parity    = Parity::None,
        .timeoutMs = OBD_TIMEOUT_MS,
    }) {}

OBDAgent::~OBDAgent() {
    stop();
}

void OBDAgent::start() {
    _thread = std::jthread([this](std::stop_token st) { run(st); });
}

void OBDAgent::stop() {
    _thread.request_stop();
    if (_thread.joinable())
        _thread.join();
}

void OBDAgent::run(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {

        // Connection phase — retry until connected or stopped
        while (!stopToken.stop_requested()) {
            if (_serial.open() && initELM327()) {
                ViewHandler::getInstance().setECUOnline(true);
                break;
            }
            _serial.close();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        // Polling phase — runs until disconnected or stopped
        while (!stopToken.stop_requested()) {
            auto pushIfValid = [&](float val, OBDEvent::OBDType type) {
                if (val >= 0.0f)
                    ViewHandler::getInstance().pushEvent(
                        std::make_unique<OBDEvent>(type, val));
            };

            float rpm = pollRPM();
            if (rpm < 0.0f) {
                // Consecutive failure — assume disconnected
                _serial.close();
                ViewHandler::getInstance().setECUOnline(false);
                break; // back to connection phase
            }

            pushIfValid(rpm,               OBDEvent::OBDType::RPM);
            pushIfValid(pollCoolantTemp(), OBDEvent::OBDType::CoolantTemp);
            pushIfValid(pollSpeed(),       OBDEvent::OBDType::Speed);
            pushIfValid(pollThrottlePos(), OBDEvent::OBDType::ThrottlePos);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    _serial.close();
    ViewHandler::getInstance().setECUOnline(false);
}

std::string OBDAgent::sendCommand(const std::string& cmd) {
    _serial.flush();

    if (!_serial.write(cmd + "\r"))
        return {};

    std::string response = _serial.readUntil('>');

    // Strip the trailing '>' and any surrounding whitespace/CR
    if (!response.empty() && response.back() == '>')
        response.pop_back();

    // Trim leading/trailing whitespace and CR/LF
    auto start = response.find_first_not_of(" \t\r\n");
    auto end   = response.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return {};

    return response.substr(start, end - start + 1);
}

bool OBDAgent::initELM327() {
    // Reset and configure for raw, compact responses
    const std::vector<std::string> initCmds = {
        "ATZ",    // Reset
        "ATE0",   // Echo off
        "ATL0",   // Linefeeds off
        "ATS0",   // Spaces off — compact response format
        "ATSP0",  // Auto-detect protocol
    };

    for (const auto& cmd : initCmds) {
        std::string resp = sendCommand(cmd);
        if (resp.empty())
            return false;

        // ATZ responds with the ELM version string, others respond "OK"
        if (cmd != "ATZ" && resp != "OK")
            return false;

        // Small delay between init commands
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return true;
}

// Parses a compact (ATS0) mode 01 response e.g. "410C1A2B"
// Returns the raw hex bytes after the mode+PID prefix as a vector of ints.
// Returns empty vector on parse failure.
static std::vector<int> parseResponse(const std::string& response, const std::string& expectedPrefix) {
    if (response.size() < expectedPrefix.size())
        return {};
    if (response.substr(0, expectedPrefix.size()) != expectedPrefix)
        return {};

    std::vector<int> bytes;
    std::string data = response.substr(expectedPrefix.size());

    // Data bytes are pairs of hex chars
    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        try {
            bytes.push_back(std::stoi(data.substr(i, 2), nullptr, 16));
        } catch (...) {
            return {};
        }
    }

    return bytes;
}

float OBDAgent::pollRPM() {
    std::string resp = sendCommand("010C");
    auto bytes = parseResponse(resp, "410C");
    if (bytes.size() < 2) return -1.0f;
    // Formula: ((A * 256) + B) / 4
    return static_cast<float>((bytes[0] * 256 + bytes[1]) / 4);
}

float OBDAgent::pollCoolantTemp() {
    std::string resp = sendCommand("0105");
    auto bytes = parseResponse(resp, "4105");
    if (bytes.size() < 1) return -1.0f;
    // Formula: A - 40 (degrees Celsius)
    return static_cast<float>(bytes[0] - 40);
}

float OBDAgent::pollSpeed() {
    std::string resp = sendCommand("010D");
    auto bytes = parseResponse(resp, "410D");
    if (bytes.size() < 1) return -1.0f;
    // Formula: A (km/h)
    return static_cast<float>(bytes[0]);
}

float OBDAgent::pollThrottlePos() {
    std::string resp = sendCommand("0111");
    auto bytes = parseResponse(resp, "4111");
    if (bytes.size() < 1) return -1.0f;
    // Formula: (A / 255) * 100 (percentage)
    return static_cast<float>(bytes[0]) * 100.0f / 255.0f;
}