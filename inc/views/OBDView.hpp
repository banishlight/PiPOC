#pragma once
#include <ViewHandler.hpp>
#include <Sensor.hpp>
#include <raylib.h>
#include <optional>

// Bar range constants — edit these to adjust gauge min/max
static constexpr float BAR_RPM_MAX      = 9500.0f;
static constexpr float BAR_SPEED_MAX    = 120.0f;   // km/h
static constexpr float BAR_THROTTLE_MAX = 100.0f;   // percent
static constexpr float BAR_COOLANT_MAX  = 120.0f;   // degrees C
static constexpr float BAR_OIL_MAX      = 130.0f;   // degrees C
static constexpr float BAR_MAF_MAX      = 20.0f;    // g/sec

// Warning thresholds
static constexpr float WARN_COOLANT_C   = 105.0f;   // degrees C
static constexpr float WARN_OIL_C       = 120.0f;   // degrees C

class OBDView : public View {
    public:
        OBDView();
        ~OBDView();
        void draw() override;
        int logic() override;
        void start() override;
        void close() override;
    private:
        void fetchEvents();

        // Hero cells
        Sensor _rpm{true, 5};
        Sensor _coolant{true, 3};

        // Secondary cells
        Sensor _speed{false};
        Sensor _throttle{false};
        Sensor _oilTemp{true, 3};
        Sensor _intakeTemp{true, 3};
        Sensor _maf{true, 8};
        Sensor _voltage{false};

        // Drawing helpers
        void drawCell(int x, int y, int w, int h,
                      const char* label, const char* value, const char* unit,
                      Color bg, Color labelCol, Color valueCol,
                      float barPct, Color barCol, bool hasBar) const;
        void drawTopBar() const;
        void drawBottomBar() const;


};