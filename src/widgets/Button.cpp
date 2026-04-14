#include <widgets/Button.hpp>

Button::Button(int x, int y, int width, int height, const std::string& label)
    : _x(x), _y(y), _width(width), _height(height), _label(label) {}

void Button::setOnClick(std::function<void()> callback) {
    _onClick = callback;
}

void Button::setColors(Color bg, Color hover, Color text) {
    _bgColor    = bg;
    _hoverColor = hover;
    _textColor  = text;
}

bool Button::containsPoint(float x, float y) const {
    return x >= _x && x <= _x + _width &&
           y >= _y && y <= _y + _height;
}

void Button::draw() {
    if (!_visible) return;
    DrawRectangle(_x, _y, _width, _height, _hovered ? _hoverColor : _bgColor);
    DrawRectangleLines(_x, _y, _width, _height, GRAY);
    int tw = MeasureText(_label.c_str(), 16);
    DrawText(_label.c_str(), _x + (_width - tw) / 2, _y + (_height - 16) / 2, 16, _textColor);
}

bool Button::handleEvent(const InputEvent& event) {
    if (!_visible) return false;

    if (event.inputType == InputEvent::InputType::TAP) {
        if (containsPoint(event.x, event.y)) {
            if (_onClick) _onClick();
            return true;
        }
    }

    return false;
}