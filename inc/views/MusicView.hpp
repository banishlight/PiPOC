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
};