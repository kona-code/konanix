#include "Magnification.h"
#include <iostream>

Magnification::Magnification(int width, int height)
    : screenWidth(width), screenHeight(height), zoomFactor(1.0f) {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // create an SDL window
    window = SDL_CreateWindow("Magnification Effect", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // create the SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

Magnification::~Magnification() {
    cleanup();
}

void Magnification::setZoomFactor(float factor) {
    zoomFactor = factor;
}

void Magnification::render() {
    // clear the window
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = { 100, 100, static_cast<int>(300 * zoomFactor), static_cast<int>(200 * zoomFactor) };
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &rect);

    // present the renderer to the screen
    SDL_RenderPresent(renderer);
}

void Magnification::cleanup() {
    // destroy the renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
