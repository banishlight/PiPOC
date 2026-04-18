#include <widgets/Button.hpp>
#include <Assets.hpp>
#include <raylib.h>

Button::Button(int x, int y, int width, int height, const std::string& label)
    : _x(x), _y(y), _width(width), _height(height), _label(label) {}

void Button::setOnClick(std::function<void()> callback) {
    _onClick = callback;
}

void Button::setSublabel(const std::string& sublabel) {
    _sublabel = sublabel;
}

void Button::setImage(Texture2D texture, Color bgTint) {
    _image       = texture;
    _imageBgTint = bgTint;
}

void Button::setColors(Color bg, Color text) {
    _bgColor   = bg;
    _textColor = text;
}

bool Button::containsPoint(float x, float y) const {
    return x >= _x && x <= _x + _width &&
           y >= _y && y <= _y + _height;
}

void Button::drawAsImage() const {
    if (_imageBgTint.a > 0)
        DrawRectangle(_x, _y, _width, _height, _imageBgTint);

    Texture2D tex = _image.value();
    int ix = _x + (_width  - tex.width)  / 2;
    int iy = _y + (_height - tex.height) / 2;
    DrawTexture(tex, ix, iy, WHITE);
}

void Button::drawAsLabel() const {
    DrawRectangle(_x, _y, _width, _height, _bgColor);
    DrawRectangleLines(_x, _y, _width, _height, GRAY);

    int labelY = _sublabel.has_value()
        ? _y + (_height / 2) - 20
        : _y + (_height - 24) / 2;

    int lw = (int)MeasureTextEx(Assets::catFont24, _label.c_str(), 24, 1).x;
    DrawTextEx(Assets::catFont24, _label.c_str(),
               {(float)(_x + (_width - lw) / 2), (float)labelY},
               24, 1, _textColor);

    if (_sublabel.has_value()) {
        int sw = (int)MeasureTextEx(Assets::catFont16, _sublabel->c_str(), 16, 1).x;
        DrawTextEx(Assets::catFont16, _sublabel->c_str(),
                   {(float)(_x + (_width - sw) / 2), (float)(_y + (_height / 2) + 8)},
                   16, 1, GRAY);
    }
}

void Button::draw() {
    if (!_visible) return;

    if (_image.has_value())
        drawAsImage();
    else
        drawAsLabel();
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