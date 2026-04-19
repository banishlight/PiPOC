#pragma once

constexpr const char* VERSION = "1.0.0";
constexpr const char* CAR_NAME = "MAZDA RX-8  13B-MSP";

// Header for storing constants related to 
// hardware and other global values.
static constexpr int DISPLAY_W = 1024;
static constexpr int DISPLAY_H = 600;

// OBD/ELM227 Consts
constexpr const char* OBD_SERIAL_DEVICE = "/tmp/obd-sim";
constexpr int         OBD_BAUD_RATE     = 38400;
constexpr int         OBD_TIMEOUT_MS    = 1000;