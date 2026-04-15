#include <agents/BluetoothAgent.hpp>
#include <iostream>
#include <chrono>
#include <thread>

BluetoothAgent::BluetoothAgent() {}

BluetoothAgent::~BluetoothAgent() {
    stop();
}

void BluetoothAgent::start() {
    _thread = std::jthread(&BluetoothAgent::run, this);
}

void BluetoothAgent::stop() {
    _thread.request_stop();
    shutdownDBus();
}

void BluetoothAgent::run(std::stop_token stopToken) {
    if (!initDBus()) {
        std::cerr << "[BT] Failed to connect to D-Bus\n";
        return;
    }

    // Try to find and connect to last paired device on startup
    if (findLastPairedDevice()) {
        if (connectDevice(_devicePath)) {
            pushConnectionEvent(BTEvent::BTType::DeviceConnected, _deviceName);
            _connected = true;
        }
    }

    while (!stopToken.stop_requested()) {
        watchConnectionState();

        if (_connected) {
            pollMPRIS();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(kPollIntervalMs));
    }
}

bool BluetoothAgent::initDBus() {
    DBusError error;
    dbus_error_init(&error);

    _dbus = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "[BT] D-Bus error: " << error.message << "\n";
        dbus_error_free(&error);
        return false;
    }

    return _dbus != nullptr;
}

void BluetoothAgent::shutdownDBus() {
    if (_dbus) {
        dbus_connection_unref(_dbus);
        _dbus = nullptr;
    }
}

bool BluetoothAgent::findLastPairedDevice() {
    if (!_dbus) return false;

    DBusError error;
    dbus_error_init(&error);

    DBusMessage* msg = dbus_message_new_method_call(
        "org.bluez",
        "/",
        "org.freedesktop.DBus.ObjectManager",
        "GetManagedObjects"
    );

    if (!msg) return false;

    DBusMessage* reply = dbus_connection_send_with_reply_and_block(
        _dbus, msg, 2000, &error
    );
    dbus_message_unref(msg);

    if (dbus_error_is_set(&error) || !reply) {
        std::cerr << "[BT] GetManagedObjects failed: " << error.message << "\n";
        dbus_error_free(&error);
        return false;
    }

    // Parse reply to find a paired device with audio sink
    DBusMessageIter iter;
    dbus_message_iter_init(reply, &iter);

    // Walk the object manager response looking for paired devices
    // This is a simplified scan — looks for first paired device
    // Full implementation would parse the nested dict properly
    bool found = false;
    if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY) {
        DBusMessageIter arrayIter;
        dbus_message_iter_recurse(&iter, &arrayIter);

        while (dbus_message_iter_get_arg_type(&arrayIter) == DBUS_TYPE_DICT_ENTRY) {
            DBusMessageIter dictIter;
            dbus_message_iter_recurse(&arrayIter, &dictIter);

            const char* path = nullptr;
            dbus_message_iter_get_basic(&dictIter, &path);

            if (path && std::string(path).find("/org/bluez/hci0/dev_") != std::string::npos) {
                _devicePath = path;
                found = true;
                break;
            }
            dbus_message_iter_next(&arrayIter);
        }
    }

    dbus_message_unref(reply);
    return found;
}

bool BluetoothAgent::connectDevice(const std::string& devicePath) {
    if (!_dbus) return false;

    DBusError error;
    dbus_error_init(&error);

    DBusMessage* msg = dbus_message_new_method_call(
        "org.bluez",
        devicePath.c_str(),
        "org.bluez.Device1",
        "Connect"
    );

    if (!msg) return false;

    DBusMessage* reply = dbus_connection_send_with_reply_and_block(
        _dbus, msg, 5000, &error
    );
    dbus_message_unref(msg);

    if (dbus_error_is_set(&error)) {
        std::cerr << "[BT] Connect failed: " << error.message << "\n";
        dbus_error_free(&error);
        return false;
    }

    if (reply) dbus_message_unref(reply);
    return true;
}

void BluetoothAgent::watchConnectionState() {
    if (_devicePath.empty() || !_dbus) return;

    DBusError error;
    dbus_error_init(&error);

    DBusMessage* msg = dbus_message_new_method_call(
        "org.bluez",
        _devicePath.c_str(),
        "org.freedesktop.DBus.Properties",
        "Get"
    );

    if (!msg) return;

    const char* iface = "org.bluez.Device1";
    const char* prop  = "Connected";
    dbus_message_append_args(msg,
        DBUS_TYPE_STRING, &iface,
        DBUS_TYPE_STRING, &prop,
        DBUS_TYPE_INVALID
    );

    DBusMessage* reply = dbus_connection_send_with_reply_and_block(
        _dbus, msg, 1000, &error
    );
    dbus_message_unref(msg);

    if (dbus_error_is_set(&error) || !reply) {
        dbus_error_free(&error);
        return;
    }

    DBusMessageIter iter, variantIter;
    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &variantIter);

    dbus_bool_t connected = false;
    dbus_message_iter_get_basic(&variantIter, &connected);
    dbus_message_unref(reply);

    if (connected && !_connected) {
        _connected = true;
        pushConnectionEvent(BTEvent::BTType::DeviceConnected, _deviceName);
    } else if (!connected && _connected) {
        _connected = false;
        pushConnectionEvent(BTEvent::BTType::DeviceDisconnected, _deviceName);
    }
}

void BluetoothAgent::pollMPRIS() {
    std::string title  = getMPRISProperty("xesam:title");
    std::string artist = getMPRISProperty("xesam:artist");

    if (!title.empty() && title != _lastTitle) {
        _lastTitle = title;
        pushTrackChanged(title, artist);
    }
}

std::string BluetoothAgent::getMPRISProperty(const std::string& property) {
    if (!_dbus) return "";

    DBusError error;
    dbus_error_init(&error);

    // Find the MPRIS player — typically org.mpris.MediaPlayer2.* 
    DBusMessage* msg = dbus_message_new_method_call(
        "org.mpris.MediaPlayer2.Player",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        "Get"
    );

    if (!msg) return "";

    const char* iface = "org.mpris.MediaPlayer2.Player";
    const char* prop  = "Metadata";
    dbus_message_append_args(msg,
        DBUS_TYPE_STRING, &iface,
        DBUS_TYPE_STRING, &prop,
        DBUS_TYPE_INVALID
    );

    DBusMessage* reply = dbus_connection_send_with_reply_and_block(
        _dbus, msg, 1000, &error
    );
    dbus_message_unref(msg);

    if (dbus_error_is_set(&error) || !reply) {
        dbus_error_free(&error);
        return "";
    }

    // Parse metadata dict for requested property
    std::string result;
    DBusMessageIter iter, variantIter, arrayIter, dictIter;
    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_recurse(&iter, &variantIter);

    if (dbus_message_iter_get_arg_type(&variantIter) == DBUS_TYPE_ARRAY) {
        dbus_message_iter_recurse(&variantIter, &arrayIter);
        while (dbus_message_iter_get_arg_type(&arrayIter) == DBUS_TYPE_DICT_ENTRY) {
            dbus_message_iter_recurse(&arrayIter, &dictIter);
            const char* key = nullptr;
            dbus_message_iter_get_basic(&dictIter, &key);
            if (key && property == key) {
                dbus_message_iter_next(&dictIter);
                DBusMessageIter valueIter;
                dbus_message_iter_recurse(&dictIter, &valueIter);
                const char* value = nullptr;
                dbus_message_iter_get_basic(&valueIter, &value);
                if (value) result = value;
                break;
            }
            dbus_message_iter_next(&arrayIter);
        }
    }

    dbus_message_unref(reply);
    return result;
}

void BluetoothAgent::pushTrackChanged(const std::string& title, const std::string& artist) {
    auto event = std::make_unique<BTEvent>();
    event->btType  = BTEvent::BTType::TrackChanged;
    event->title   = title;
    event->artist  = artist;
    ViewHandler::getInstance().pushEvent(std::move(event));
}

void BluetoothAgent::pushConnectionEvent(BTEvent::BTType type, const std::string& deviceName) {
    auto event = std::make_unique<BTEvent>();
    event->btType     = type;
    event->deviceName = deviceName;
    ViewHandler::getInstance().pushEvent(std::move(event));
}

void BluetoothAgent::pushPlaybackState(bool playing) {
    auto event = std::make_unique<BTEvent>();
    event->btType  = BTEvent::BTType::PlaybackStateChanged;
    event->playing = playing;
    ViewHandler::getInstance().pushEvent(std::move(event));
}