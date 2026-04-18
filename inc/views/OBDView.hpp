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
 
        // Main cells
        std::optional<float> _rpm;
        std::optional<float> _coolant;
 
        // Secondary cells
        std::optional<float> _speed;
        std::optional<float> _throttle;
        std::optional<float> _oilTemp;
        std::optional<float> _intakeTemp;
        std::optional<float> _maf;
        std::optional<float> _voltage;
 
        // Drawing helpers
        void drawCell(int x, int y, int w, int h,
                      const char* label, const char* value, const char* unit,
                      Color bg, Color labelCol, Color valueCol,
                      float barPct, Color barCol, bool hasBar) const;
 
        void drawTopBar() const;
        void drawBottomBar() const;
};