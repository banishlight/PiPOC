#pragma once
#include <raylib.h>
#include <ViewHandler.hpp>

class Widget {
public:
    virtual ~Widget() = default;

    virtual void draw() = 0;
    virtual bool handleEvent(const InputEvent& event) = 0;  // returns true if consumed

    bool isVisible() const { return _visible; }
    void setVisible(bool v) { _visible = v; }

protected:
    bool _visible = true;
};