#pragma once
#pragma once
#include <windows.h>

class MagnifierManager {
public:
    MagnifierManager();
    ~MagnifierManager();

    // initialize the magnifier window covering the entire screen
    bool Initialize(HINSTANCE hInstance);
    // apply a scale transform (for example, 0.9 for 90% scale)
    bool ApplyScaleTransform(float scale);
    // remove any transform (restore 100% scale)
    bool RemoveScaleTransform();
    // show the magnifier overlay
    void Show();
    // hide the magnifier overlay
    void Hide();

private:
    HWND m_hwndMagnifier;
    HWND m_hwndOverlay;
    bool m_initialized;
};
