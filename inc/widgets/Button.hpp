#pragma once
#include <widgets/Widget.hpp>
#include <string>
#include <functional>
#include <optional>
#include <raylib.h>

class Button : public Widget {
public:
    Button(int x, int y, int width, int height, const std::string& label);

    void draw() override;
    bool handleEvent(const InputEvent& event) override;

    // Optional sublabel — ignored if an image is set
    void setSublabel(const std::string& sublabel);

    // Sets an image — disables label/sublabel rendering
    // Pass a transparent color for no background fill
    void setImage(Texture2D texture, Color bgTint = {0, 0, 0, 0});

    void setOnClick(std::function<void()> callback);
    void setColors(Color bg, Color text);

private:
    bool containsPoint(float x, float y) const;
    void drawAsImage() const;
    void drawAsLabel() const;

    int         _x, _y, _width, _height;
    std::string _label;
    std::optional<std::string>  _sublabel;
    std::optional<Texture2D>    _image;
    Color                       _imageBgTint = {0, 0, 0, 0};

    std::function<void()> _onClick;

    Color _bgColor   = {30,  30,  30,  255};
    Color _textColor = WHITE;
};