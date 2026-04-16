#include <Assets.hpp>
#include <raylib.h>

namespace Assets {
    Font mainFont;
    Font catFont;

    void load() {
        mainFont = LoadFontEx("assets/roboto.ttf", 64, nullptr, 0);
        catFont = LoadFontEx("assets/Rajdhani-Regular", 64, nullptr, 0);
    }

    void unload() {
        UnloadFont(mainFont);
        UnloadFont(catFont);
    }
}