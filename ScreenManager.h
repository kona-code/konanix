#pragma once
#include <windows.h>

class ScreenManager {
public:
    ScreenManager();
    ~ScreenManager();

    // shrink the screen work area to 90% of its original size
    void ShrinkScreen();

    // restore the screen work area to its original state
    void RestoreScreen();

private:
    RECT originalRect;  // stores the original screen work area
    bool isShrunk;
};