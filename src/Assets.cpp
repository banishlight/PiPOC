#include <Assets.hpp>
#include <raylib.h>

namespace Assets {
    Font mainFont;
    Font catFont;
    Font catFont16;
    Font catFont24;
    Font catFont52;
    Texture2D gearIcon;

    void load() {
        mainFont = LoadFontEx("assets/roboto.ttf", 64, nullptr, 0);
        catFont = LoadFontEx("assets/Rajdhani-Regular.ttf", 64, nullptr, 0);
        catFont16 = LoadFontEx("assets/Rajdhani-Regular.ttf", 16, nullptr, 0);
        catFont24 = LoadFontEx("assets/Rajdhani-Regular.ttf", 24, nullptr, 0);
        catFont52 = LoadFontEx("assets/Rajdhani-Regular.ttf", 52, nullptr, 0);
        gearIcon  = LoadTexture("assets/gear.png");
    }

    void unload() {
        UnloadFont(mainFont);
        UnloadFont(catFont);
        UnloadFont(catFont16);
        UnloadFont(catFont24);
        UnloadFont(catFont52);
        UnloadTexture(gearIcon);
    }
}