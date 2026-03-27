#pragma once
#include <ViewHandler.hpp>

class MainView : public View {
public:
    MainView();
    ~MainView();

    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void draw() override;
    bool handleEvent(const AgentEvent& event) override;
};