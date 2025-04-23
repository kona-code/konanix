#include "ScreenManager.h"
#include <windows.h>

ScreenManager::ScreenManager() : isShrunk(false) {
    // get the original work area
    SystemParametersInfo(SPI_GETWORKAREA, 0, &originalRect, 0);
}

ScreenManager::~ScreenManager() {
    // restore screen if the object is destructed and the screen is shrunk
    if (isShrunk) {
        RestoreScreen();
    }
}

void ScreenManager::ShrinkScreen() {
    if (isShrunk) return;  // prevent re-shrinking if already shrunk

    // get current screen width and height
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // calculate new dimensions (90% of original size)
	int newWidth = static_cast<int>(screenWidth * 0.9);
    int newHeight = static_cast<int>(screenHeight * 0.9);
    int centerX = (screenWidth - newWidth) / 2;
    int centerY = (screenHeight - newHeight) / 2;

    // set the new work area
    RECT newWorkArea = { centerX, centerY, centerX + newWidth, centerY + newHeight };
    SystemParametersInfo(SPI_SETWORKAREA, 0, &newWorkArea, SPIF_SENDCHANGE);

    isShrunk = true;
}

void ScreenManager::RestoreScreen() {
    if (!isShrunk) return;  // only restore if the screen has been shrunk

    // restore the original screen work area
    SystemParametersInfo(SPI_SETWORKAREA, 0, &originalRect, SPIF_SENDCHANGE);
    isShrunk = false;
}