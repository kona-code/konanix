#pragma once
#include <windows.h>
#include <magnification.h>
#include <iostream>
#include <thread>
#include <chrono>

#pragma comment(lib, "Magnification.lib")

// manages screen magnification using the windows magnification api
class MagnifierManager {
public:
    MagnifierManager();
    ~MagnifierManager();

    // initialize overlay window and load the background image
    bool Initialize(HINSTANCE hInstance);
    // show/hide the overlay window
    void Show();
    void Hide();
    // apply a scale transform with centered translation
    bool ApplyScaleTransform(float scale);
    void AnimateScale(float targetScale, int durationMs);
    bool IsInitialized() const;

private:
    // draws the background image
    static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwndMagnifier;   // handle to the magnifier control
    HWND m_hwndOverlay;     // handle to the overlay window
    bool m_initialized;     // whether initialization succeeded
    float m_currentScale;   // current applied scale factor
    float m_targetScale;    // target scale factor (set by animation)
    int m_animationSteps;   // total number of steps in the animation
    int m_currentStep;      // current animation step

    // screen dimensions computed once
    int m_screenWidth;
    int m_screenHeight;

    // background image for the overlay
    HBITMAP m_hBackground;

    bool LoadBackgroundImage(const wchar_t* imagePath);
};
