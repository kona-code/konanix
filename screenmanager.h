#pragma once

#include <windows.h>

// class that manages the screen manipulation
class ScreenManager {
public:
    ScreenManager();
    void ShrinkAndCenterScreen();
    void RestoreScreen();

private:
    // original work area is stored here if needed
    RECT originalRect;
    bool isShrunk;
};
