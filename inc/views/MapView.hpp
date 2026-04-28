#pragma once
#include <ViewHandler.hpp>
#include <widgets/Topbar.hpp>
#include <widgets/Bottombar.hpp>

class MapView : public View {
public:
    MapView();
    ~MapView();
    void draw()  override;
    int  logic() override;
    void start() override;
    void close() override;

private:
    void _fetchEvents();

    TopBar    _topBar;
    BottomBar _bottomBar{true};
};