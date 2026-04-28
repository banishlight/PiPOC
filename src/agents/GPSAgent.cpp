#include <agents/GPSAgent.hpp>
#include <ViewHandler.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>

GPSAgent::GPSAgent()
    : _serial(SerialConfig{
        .device    = GPS_DEVICE,
        .baudRate  = GPS_BAUD_RATE,
        .dataBits  = 8,
        .stopBits  = 1,
        .parity    = Parity::None,
        .timeoutMs = GPS_TIMEOUT_MS,
    }) {}

GPSAgent::~GPSAgent() {
    stop();
}

void GPSAgent::start() {
    _thread = std::jthread([this](std::stop_token st) { run(st); });
}

void GPSAgent::stop() {
    _thread.request_stop();
    if (_thread.joinable())
        _thread.join();
}

void GPSAgent::run(std::stop_token stopToken) {
    // Retry loop — keep trying to open port until successful or stopped
    while (!stopToken.stop_requested()) {
        if (_serial.open()) break;
        std::cerr << "[GPS] Could not open " << GPS_DEVICE << ", retrying in 3 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    if (stopToken.stop_requested()) return;

    std::cout << "[GPS] Serial port open, reading NMEA...\n";

    while (!stopToken.stop_requested()) {
        std::string sentence = readSentence();
        if (sentence.empty()) continue;
        if (!validateChecksum(sentence)) continue;

        if (sentence.rfind("$GPRMC", 0) == 0)
            parseGPRMC(sentence);
        else if (sentence.rfind("$GPGGA", 0) == 0)
            parseGPGGA(sentence);
    }

    _serial.close();
}

std::string GPSAgent::readSentence() {
    std::string sentence = _serial.readUntil('\n');

    // Strip trailing CR/LF
    while (!sentence.empty() && (sentence.back() == '\r' || sentence.back() == '\n'))
        sentence.pop_back();

    // Must start with '$'
    if (sentence.empty() || sentence.front() != '$')
        return {};

    return sentence;
}

bool GPSAgent::validateChecksum(const std::string& sentence) const {
    // Format: $<data>*<checksum>
    auto star = sentence.find('*');
    if (star == std::string::npos || star + 3 > sentence.size())
        return false;

    // XOR all bytes between $ and *
    uint8_t calculated = 0;
    for (size_t i = 1; i < star; i++)
        calculated ^= static_cast<uint8_t>(sentence[i]);

    // Parse the two hex checksum digits
    try {
        uint8_t expected = static_cast<uint8_t>(
            std::stoi(sentence.substr(star + 1, 2), nullptr, 16));
        return calculated == expected;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> GPSAgent::splitFields(const std::string& sentence) const {
    std::vector<std::string> fields;
    std::stringstream ss(sentence);
    std::string field;
    while (std::getline(ss, field, ','))
        fields.push_back(field);
    return fields;
}

double GPSAgent::nmeaToDecimal(const std::string& value, const std::string& direction) const {
    if (value.empty()) return 0.0;

    // NMEA format: DDDMM.MMMM or DDMM.MMMM
    double raw = std::stod(value);
    int degrees = static_cast<int>(raw / 100);
    double minutes = raw - (degrees * 100);
    double decimal = degrees + (minutes / 60.0);

    if (direction == "S" || direction == "W")
        decimal = -decimal;

    return decimal;
}

// $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
// Fields: 0=type 1=time 2=status 3=lat 4=N/S 5=lon 6=E/W
//         7=speed(knots) 8=heading 9=date ...
void GPSAgent::parseGPRMC(const std::string& sentence) {
    auto fields = splitFields(sentence);
    if (fields.size() < 9) return;

    bool valid = (fields[2] == "A");
    _data.hasFix = valid;

    if (valid) {
        _data.lat     = nmeaToDecimal(fields[3], fields[4]);
        _data.lon     = nmeaToDecimal(fields[5], fields[6]);
        _data.speed   = std::stof(fields[7]) * 1.852f; // knots to km/h
        _data.heading = fields[8].empty() ? 0.0f : std::stof(fields[8]);
    }

    ViewHandler::getInstance().setGPSData(_data);
}

// $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,...
// Fields: 0=type 1=time 2=lat 3=N/S 4=lon 5=E/W 6=fix 7=satellites
void GPSAgent::parseGPGGA(const std::string& sentence) {
    auto fields = splitFields(sentence);
    if (fields.size() < 8) return;

    try {
        _data.satellites = std::stoi(fields[7]);
    } catch (...) {
        _data.satellites = 0;
    }

    ViewHandler::getInstance().setGPSData(_data);
}