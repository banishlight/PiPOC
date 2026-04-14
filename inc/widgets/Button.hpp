#pragma once
#include <widgets/Widget.hpp>
#include <string>
#include <functional>

class Button : public Widget {
public:
    Button(int x, int y, int width, int height, const std::string& label);

    void draw() override;
    bool handleEvent(const InputEvent& event) override;

    void setOnClick(std::function<void()> callback);
    void setColors(Color bg, Color hover, Color text);

private:
    bool containsPoint(float x, float y) const;

    int _x, _y, _width, _height;
    std::string _label;
    std::function<void()> _onClick;

    Color _bgColor    = {30, 30, 30, 255};
    Color _hoverColor = {60, 60, 60, 255};
    Color _textColor  = WHITE;
    bool  _hovered    = false;
};