#pragma once
#include <ViewHandler.hpp>
#include <SerialPort.hpp>
#include <thread>
#include <string>
#include <vector>

class GPSAgent {
public:
    GPSAgent();
    ~GPSAgent();

    GPSAgent(const GPSAgent&)            = delete;
    GPSAgent& operator=(const GPSAgent&) = delete;

    void start();
    void stop();

private:
    void run(std::stop_token stopToken);

    // Reads a single NMEA sentence from the serial port
    std::string readSentence();

    // Validates the NMEA checksum — returns true if valid
    bool validateChecksum(const std::string& sentence) const;

    // Parsers for specific sentence types
    void parseGPRMC(const std::string& sentence);
    void parseGPGGA(const std::string& sentence);

    // Splits a comma separated NMEA sentence into fields
    std::vector<std::string> splitFields(const std::string& sentence) const;

    // Converts NMEA lat/lon format (DDMM.MMMM) to decimal degrees
    double nmeaToDecimal(const std::string& value, const std::string& direction) const;

    std::jthread _thread;
    SerialPort   _serial;
    GPSData      _data;

    static constexpr const char* GPS_DEVICE    = "/dev/serial0";
    static constexpr int         GPS_BAUD_RATE = 9600;
    static constexpr int         GPS_TIMEOUT_MS = 2000;
};