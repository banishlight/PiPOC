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

void Button::setColors(Color bg, Color hover, Color fill, Color text) {
    _bgColor    = bg;
    _hoverColor = hover;
    _fillColor  = fill;
    _textColor  = text;
}

void Button::setAnimationStyle(AnimationStyle style, float duration) {
    _animStyle    = style;
    _animDuration = duration;
}

bool Button::containsPoint(float x, float y) const {
    return x >= _x && x <= _x + _width &&
           y >= _y && y <= _y + _height;
}

void Button::updateAnimation() {
    if (!_animating) return;

    _animTimer += GetFrameTime();

    if (_animTimer >= _animDuration) {
        _animTimer = 0.0f;
        _animating = false;
        _completed = true; // signal completion — caller polls this
    }
}

bool Button::pollCompleted() {
    if (_completed) {
        _completed = false;
        return true;
    }
    return false;
}

void Button::fireOnClick() {
    if (_onClick) _onClick();
}

void Button::draw() {
    if (!_visible) return;

    updateAnimation();

    switch (_animStyle) {
        case AnimationStyle::SweepFill: {
            DrawRectangle(_x, _y, _width, _height, _bgColor);

            if (_animating) {
                float t   = _animTimer / _animDuration;
                t         = t > 1.0f ? 1.0f : t;
                int fillW = (int)(_width * t);
                DrawRectangle(_x, _y, fillW, _height, _fillColor);
            }

            Color topLine = _animating ? _fillColor : GRAY;
            DrawRectangle(_x, _y, _width, 2, topLine);
            DrawRectangleLines(_x, _y, _width, _height, GRAY);
            break;
        }

        case AnimationStyle::None:
        default:
            DrawRectangle(_x, _y, _width, _height, _bgColor);
            DrawRectangleLines(_x, _y, _width, _height, GRAY);
            break;
    }

    // Main label
    int labelY = _sublabel.has_value()
        ? _y + (_height / 2) - 20
        : _y + (_height - 24) / 2;

    int lw = (int)MeasureTextEx(Assets::catFont24, _label.c_str(), 24, 1).x;
    Color labelCol = _animating ? WHITE : _textColor;
    DrawTextEx(Assets::catFont24, _label.c_str(),
               {(float)(_x + (_width - lw) / 2), (float)labelY},
               24, 1, labelCol);

    // Optional sublabel
    if (_sublabel.has_value()) {
        Color subCol = _animating ? (Color){255, 200, 200, 255} : GRAY;
        int sw = (int)MeasureTextEx(Assets::catFont16, _sublabel->c_str(), 16, 1).x;
        DrawTextEx(Assets::catFont16, _sublabel->c_str(),
                   {(float)(_x + (_width - sw) / 2), (float)(_y + (_height / 2) + 8)},
                   16, 1, subCol);
    }
}

bool Button::handleEvent(const InputEvent& event) {
    if (!_visible || _animating) return false;

    if (event.inputType == InputEvent::InputType::TAP) {
        if (containsPoint(event.x, event.y)) {
            if (_animStyle == AnimationStyle::None) {
                // No animation — fire immediately since no view switch is involved
                if (_onClick) _onClick();
            } else {
                _animating = true;
                _animTimer = 0.0f;
            }
            return true;
        }
    }

    return false;
}