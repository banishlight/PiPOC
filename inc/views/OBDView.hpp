#pragma once
#include <ViewHandler.hpp>
#include <raylib.h>

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
        Font _font;
        const std::string _fontPath = "assets/roboto.otf";
};