#pragma once
#include <string>
#include <cstdint>

// Parity options for serial port configuration
enum class Parity {
    None,
    Even,
    Odd
};

// Configuration for opening a serial port
struct SerialConfig {
    std::string device;         // e.g. "/dev/ttyUSB0"
    int         baudRate;       // e.g. 38400
    int         dataBits;       // typically 8
    int         stopBits;       // typically 1
    Parity      parity;         // typically None
    int         timeoutMs;      // read timeout in milliseconds
};

class SerialPort {
public:
    explicit SerialPort(const SerialConfig& config);
    ~SerialPort();

    // Not copyable — owns a file descriptor
    SerialPort(const SerialPort&)            = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    // Movable
    SerialPort(SerialPort&& other) noexcept;
    SerialPort& operator=(SerialPort&& other) noexcept;

    // Opens the serial port. Returns true on success.
    bool open();

    // Closes the serial port if open.
    void close();

    // Returns true if the port is currently open.
    bool isOpen() const;

    // Writes all bytes of data to the port.
    // Returns true if all bytes were written successfully.
    bool write(const std::string& data);

    // Reads bytes from the port until terminator is encountered or timeout expires.
    // The terminator character is included in the returned string.
    // Returns an empty string on timeout or error.
    std::string readUntil(char terminator);

    // Reads up to maxBytes from the port, blocking up to the configured timeout.
    // Returns however many bytes were read (may be fewer than maxBytes).
    std::string read(size_t maxBytes);

    // Discards any unread data in the input buffer.
    void flush();

    // Returns the last system error message, if any.
    std::string lastError() const;

private:
    SerialConfig config_;
    int          fd_;           // POSIX file descriptor, -1 if not open
    std::string  lastError_;

    // Applies termios settings to fd_ based on config_.
    // Called internally by open().
    bool applyConfig();
};