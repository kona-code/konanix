#pragma once

#include <SDL2/SDL.h>

#pragma comment(lib, "SetupAPI.lib")  // for HID device functions
#pragma comment(lib, "Winmm.lib")     // for multimedia functions (waveOut, timeBeginPeriod)
#pragma comment(lib, "Imm32.lib")     // for input Method Manager (IME) functions
#pragma comment(lib, "Cfgmgr32.lib")  // for configuration Manager (CM) functions


class Magnification {
public:
    Magnification(int width, int height);
    ~Magnification();

    void setZoomFactor(float factor);
    void render();

    // cleanup SDL resources
    void cleanup();

private:
    int screenWidth, screenHeight;
    float zoomFactor;
    SDL_Window* window;
    SDL_Renderer* renderer;
};
