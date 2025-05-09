/*
#pragma once
#include <windows.h>
#include <magnification.h>
#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <atomic>
#include "Settings.h"

#pragma comment(lib, "Magnification.lib")

// manages screen magnification using the windows magnification api
class MagnifierManager {
public:
    MagnifierManager();
    ~MagnifierManager();

    HWND GetOverlayWindow() const { return m_hwndOverlay; }

    bool Initialize(HINSTANCE hInstance);

    void Show();
    void Hide();

    bool ApplyScaleTransform(float scale);
    // animated zoom from current scale to target scale
    void AnimateScale(float targetScale, int durationMs);
    // continuously refresh/render content
    void StartRefreshing();
    void StopRefreshing();

    bool IsInitialized() const;
    void AddWidget(int x, int y, int width, int height, std::function<void(HDC)> paintCallback);

    struct OverlayWidget {
        HWND hwnd;
        RECT bounds;
        std::function<void(HDC)> paintCallback;
    };

private:

    Settings settings;
    // helper to update the source rectangle of the magnifier control
    void Refresh();

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

    // atomic flag to control refresh thread
    std::atomic<bool> m_refreshing;

	// stores overlay widgets
    std::vector<OverlayWidget> m_widgets;
};
*/