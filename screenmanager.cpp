#include "screenmanager.h"
#include <Windows.h>

ScreenManager::ScreenManager() {}

void ScreenManager::ShrinkAndCenterScreen() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Shrink screen by 10%
    int newWidth = static_cast<int>(screenWidth * 0.9);
    int newHeight = static_cast<int>(screenHeight * 0.9);

    // Center the screen
    int centerX = (screenWidth - newWidth) / 2;
    int centerY = (screenHeight - newHeight) / 2;

    // Apply the new size and position to the desktop
    SetWindowPos(GetDesktopWindow(), HWND_TOPMOST, centerX, centerY, newWidth, newHeight, SWP_NOACTIVATE);
}

void ScreenManager::RestoreScreen() {
    // Restore the screen to original size
    SetWindowPos(GetDesktopWindow(), HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOACTIVATE);
}
