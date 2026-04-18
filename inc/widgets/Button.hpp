#pragma once
#include <widgets/Widget.hpp>
#include <string>
#include <functional>
#include <optional>
#include <raylib.h>

class Button : public Widget {
public:
    enum class AnimationStyle {
        None,       // fires onClick immediately on tap
        SweepFill,  // fill sweeps left to right, onClick fires after completion
    };

    Button(int x, int y, int width, int height, const std::string& label);

    void draw() override;
    bool handleEvent(const InputEvent& event) override;

    // Optional sublabel — ignored if an image is set
    void setSublabel(const std::string& sublabel);

    // Sets an image — disables label/sublabel rendering and animation
    // Pass a transparent color to draw the image with no background fill
    void setImage(Texture2D texture, Color bgTint = {0, 0, 0, 0});

    void setOnClick(std::function<void()> callback);
    void setColors(Color bg, Color hover, Color fill, Color text);
    void setAnimationStyle(AnimationStyle style, float duration = 0.15f);

    bool isAnimating() const { return _animating; }
    bool pollCompleted();
    void fireOnClick();

private:
    bool containsPoint(float x, float y) const;
    void updateAnimation();
    void drawAsImage() const;
    void drawAsLabel();

    int         _x, _y, _width, _height;
    std::string _label;
    std::optional<std::string>  _sublabel;
    std::optional<Texture2D>    _image;
    Color                       _imageBgTint = {0, 0, 0, 0};

    std::function<void()> _onClick;

    Color _bgColor    = {30,  30,  30,  255};
    Color _hoverColor = {60,  60,  60,  255};
    Color _fillColor  = {255, 40,  0,   255};
    Color _textColor  = WHITE;

    AnimationStyle _animStyle    = AnimationStyle::None;
    float          _animDuration = 0.15f;
    float          _animTimer    = 0.0f;
    bool           _animating    = false;
    bool           _completed    = false;
};