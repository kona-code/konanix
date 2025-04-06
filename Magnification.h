#pragma once
#include <windows.h>
#include <magnification.h>
#include <math.h>

class MagnifierManager {
public:
    MagnifierManager();
    ~MagnifierManager();

    bool Initialize(HINSTANCE hInstance);
    bool IsInitialized() const;
    bool ApplyScaleTransform(float scale);
    bool RemoveScaleTransform();
    void Show();
    void Hide();

    void AnimateScale(float targetScale, int durationMs = 100);

private:
    HWND m_hwndMagnifier;
    HWND m_hwndOverlay;
    bool m_initialized;
    float m_currentScale;
    float m_targetScale;
    int m_animationSteps;
    int m_currentStep;

    // screen dimensions
    int m_screenWidth;
    int m_screenHeight;

    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void StepScale();
};