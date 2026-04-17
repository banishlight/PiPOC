#pragma once
#include <ViewHandler.hpp>
#include <widgets/Button.hpp>
#include <memory>
#include <vector>

class MapView : public View {
public:
    MapView();
    ~MapView();
    void start()  override;
    void close()  override;
    void draw()   override;
    int  logic()  override;

private:
    void _fetchEvents();
    std::vector<std::unique_ptr<Button>> _widgets;
};