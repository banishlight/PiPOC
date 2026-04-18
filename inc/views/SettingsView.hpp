#pragma once
#include <ViewHandler.hpp>
#include <widgets/Button.hpp>
#include <memory>
#include <vector>

class SettingsView : public View {
public:
    SettingsView();
    ~SettingsView();
    void start()  override;
    void close()  override;
    void draw()   override;
    int  logic()  override;

private:
    void _fetchEvents();
    std::vector<std::unique_ptr<Button>> _widgets;
    // Held separately so we can poll it for deferred switch
    Button* _backBtn = nullptr;
};