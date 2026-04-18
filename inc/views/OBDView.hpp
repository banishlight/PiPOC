#pragma once
#include <ViewHandler.hpp>
#include <raylib.h>
#include <optional>
 
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
 
        // OBD data — negative means no data received yet
        std::optional<float> _rpm;
        std::optional<float> _coolant;
        std::optional<float> _speed;
        std::optional<float> _throttle;
};