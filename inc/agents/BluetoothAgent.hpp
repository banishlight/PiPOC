#pragma once
#include <ViewHandler.hpp>
#include <atomic>
#include <string>
#include <thread>
#include <dbus/dbus.h>

class BluetoothAgent {
public:
    BluetoothAgent();
    ~BluetoothAgent();

    void start();
    void stop();

private:
    void run(std::stop_token stopToken);

    // D-Bus connection
    bool        initDBus();
    void        shutdownDBus();

    // BlueZ device management
    bool        findLastPairedDevice();
    bool        connectDevice(const std::string& devicePath);
    void        watchConnectionState();
    void        setDeviceName(const std::string& name);

    // MPRIS metadata
    void        pollMPRIS();
    std::string getMPRISProperty(const std::string& property);

    // Event helpers
    void        pushTrackChanged(const std::string& title, const std::string& artist);
    void        pushConnectionEvent(BTEvent::BTType type, const std::string& deviceName);
    void        pushPlaybackState(bool playing);

    std::jthread  _thread;
    DBusConnection* _dbus        = nullptr;
    std::string   _devicePath;
    std::string   _deviceName;
    std::string   _lastTitle;
    bool          _connected     = false;
    std::string   _playerPath;
    bool          _playing = false;

    static constexpr int kPollIntervalMs = 1000;  // MPRIS poll rate
    static constexpr const char* BT_NAME = "CarPi";
};