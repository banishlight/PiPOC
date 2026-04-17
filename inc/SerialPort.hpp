#pragma once
#include <string>

class SerialPort {
public:
    SerialPort(const std::string& device, int baudRate);
    ~SerialPort();

    bool open();
    void close();
    bool isOpen() const;

    bool write(const std::string& cmd);
    std::string read();
    std::string sendCommand(const std::string& cmd);

private:
    std::string device_;
    int baudRate_;
    int fd_;
};