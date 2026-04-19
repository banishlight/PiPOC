#pragma once
#include <ViewHandler.hpp>
#include <Sensor.hpp>
#include <widgets/Topbar.hpp>
#include <widgets/Bottombar.hpp>
#include <raylib.h>
#include <optional>

// Warning thresholds
static constexpr float WARN_COOLANT_C = 105.0f;   // degrees C
static constexpr float WARN_OIL_C     = 120.0f;   // degrees C

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
        Sensor _speed{false, 3};
        Sensor _throttle{false, 3};
        Sensor _oilTemp{true, 3};
        Sensor _intakeTemp{true, 3};
        Sensor _maf{true, 8};
        Sensor _voltage{false, 3};

        TopBar    _topBar;
        BottomBar _bottomBar{true};
};