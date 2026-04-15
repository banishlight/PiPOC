#pragma once
#include <ViewHandler.hpp>
#include <widgets/Widget.hpp>
#include <vector>

class MusicView : public View {
    public:
        MusicView();
        ~MusicView();
        void draw() override;
        int logic() override;
        void start() override;
        void close() override;
    private:
        void _fetchEvents();
        std::vector<std::unique_ptr<Widget>> _widgets;
        std::string _title      = "No track";
        std::string _artist     = "No artist";
        std::string _deviceName = "Not connected";
        bool        _connected  = false;
        Font _font;
        const std::string _fontPath = "assets/roboto.ttf";
};