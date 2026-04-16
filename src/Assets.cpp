#include <Assets.hpp>
#include <raylib.h>

namespace Assets {
    Font mainFont;
    Font catFont;
    Font catFont16;
    Font catFont24;
    Font catFont32;
    Font catFont52;
    Texture2D gearIcon;

    void load() {
        mainFont = LoadFontEx("assets/roboto.ttf", 64, nullptr, 0);
        catFont = LoadFontEx("assets/Rajdhani-Regular.ttf", 64, nullptr, 0);
        catFont16 = LoadFontEx("assets/fonts/rajdhani.ttf", 16, nullptr, 0);
        catFont24 = LoadFontEx("assets/fonts/rajdhani.ttf", 24, nullptr, 0);
        catFont32 = LoadFontEx("assets/fonts/rajdhani.ttf", 32, nullptr, 0);
        catFont52 = LoadFontEx("assets/fonts/rajdhani.ttf", 52, nullptr, 0);
        gearIcon  = LoadTexture("assets/gear.png");
    }

    void unload() {
        UnloadFont(mainFont);
        UnloadFont(catFont);
        UnloadFont(catFont16);
        UnloadFont(catFont24);
        UnloadFont(catFont32);
        UnloadFont(catFont52);
        UnloadTexture(gearIcon);
    }
}