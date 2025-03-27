#pragma once
#include <windows.h>

// class that manages screen manipulation
class ScreenManager {
public:
    ScreenManager();
    void ShrinkAndCenterScreen();
    void RestoreScreen();

private:
    RECT originalRect;
    bool isShrunk;
};
