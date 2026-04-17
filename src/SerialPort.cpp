#include <SerialPort.hpp>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

static speed_t baudToSpeed(int baud) {
    switch (baud) {
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        default:     return B0; // Invalid — caller should handle
    }
}

SerialPort::SerialPort(const SerialConfig& config)
    : config_(config), fd_(-1) {}

SerialPort::~SerialPort() {
    close();
}

SerialPort::SerialPort(SerialPort&& other) noexcept
    : config_(std::move(other.config_)), fd_(other.fd_), lastError_(std::move(other.lastError_)) {
    other.fd_ = -1;
}

SerialPort& SerialPort::operator=(SerialPort&& other) noexcept {
    if (this != &other) {
        close();
        config_    = std::move(other.config_);
        fd_        = other.fd_;
        lastError_ = std::move(other.lastError_);
        other.fd_  = -1;
    }
    return *this;
}

bool SerialPort::open() {
    if (isOpen()) {
        lastError_ = "Port is already open";
        return false;
    }

    fd_ = ::open(config_.device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) {
        lastError_ = std::string("Failed to open ") + config_.device + ": " + std::strerror(errno);
        return false;
    }

    // Switch to blocking I/O now that the port is open
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);

    if (!applyConfig()) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    return true;
}

void SerialPort::close() {
    if (isOpen()) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool SerialPort::isOpen() const {
    return fd_ >= 0;
}

// Configuration (termios)

bool SerialPort::applyConfig() {
    termios tty{};
    if (tcgetattr(fd_, &tty) != 0) {
        lastError_ = std::string("tcgetattr failed: ") + std::strerror(errno);
        return false;
    }

    // Baud rate
    speed_t speed = baudToSpeed(config_.baudRate);
    if (speed == B0) {
        lastError_ = "Unsupported baud rate: " + std::to_string(config_.baudRate);
        return false;
    }
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    // Data bits
    tty.c_cflag &= ~CSIZE;
    switch (config_.dataBits) {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8:
        default: tty.c_cflag |= CS8; break;
    }

    // Stop bits
    if (config_.stopBits == 2)
        tty.c_cflag |= CSTOPB;
    else
        tty.c_cflag &= ~CSTOPB;

    // Parity
    switch (config_.parity) {
        case Parity::Even:
            tty.c_cflag |=  PARENB;
            tty.c_cflag &= ~PARODD;
            break;
        case Parity::Odd:
            tty.c_cflag |= PARENB;
            tty.c_cflag |= PARODD;
            break;
        case Parity::None:
        default:
            tty.c_cflag &= ~PARENB;
            break;
    }

    // Hardware flow control off, enable receiver, local mode
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |=  CREAD | CLOCAL;

    // Raw input (no special processing)
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);

    // Raw output
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // Read timeout: VTIME is in tenths of a second, VMIN = 0 means return after timeout
    // even if no bytes were read.
    int tenths = (config_.timeoutMs + 99) / 100; // round up to nearest tenth-second
    tty.c_cc[VTIME] = static_cast<cc_t>(tenths > 255 ? 255 : tenths);
    tty.c_cc[VMIN]  = 0;

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        lastError_ = std::string("tcsetattr failed: ") + std::strerror(errno);
        return false;
    }

    return true;
}

bool SerialPort::write(const std::string& data) {
    if (!isOpen()) {
        lastError_ = "Port is not open";
        return false;
    }

    ssize_t total   = static_cast<ssize_t>(data.size());
    ssize_t written = 0;

    while (written < total) {
        ssize_t n = ::write(fd_, data.c_str() + written, total - written);
        if (n < 0) {
            lastError_ = std::string("write failed: ") + std::strerror(errno);
            return false;
        }
        written += n;
    }

    return true;
}

std::string SerialPort::readUntil(char terminator) {
    if (!isOpen()) {
        lastError_ = "Port is not open";
        return {};
    }

    std::string result;
    char        byte;

    while (true) {
        ssize_t n = ::read(fd_, &byte, 1);
        if (n < 0) {
            lastError_ = std::string("read failed: ") + std::strerror(errno);
            return {};
        }
        if (n == 0) {
            // Timeout — no data within the configured window
            lastError_ = "Read timeout";
            return {};
        }
        result += byte;
        if (byte == terminator)
            break;
    }

    return result;
}

std::string SerialPort::read(size_t maxBytes) {
    if (!isOpen()) {
        lastError_ = "Port is not open";
        return {};
    }

    std::string buffer(maxBytes, '\0');
    ssize_t n = ::read(fd_, buffer.data(), maxBytes);

    if (n < 0) {
        lastError_ = std::string("read failed: ") + std::strerror(errno);
        return {};
    }

    buffer.resize(static_cast<size_t>(n));
    return buffer;
}

void SerialPort::flush() {
    if (isOpen())
        tcflush(fd_, TCIFLUSH);
}

std::string SerialPort::lastError() const {
    return lastError_;
}