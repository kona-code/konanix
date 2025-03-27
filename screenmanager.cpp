#include "screenmanager.h"
#include <windows.h>

ScreenManager::ScreenManager() : isShrunk(false)
{
    // get the original work area
    SystemParametersInfo(SPI_GETWORKAREA, 0, &originalRect, 0);
}

void ScreenManager::ShrinkAndCenterScreen()
{
    if (isShrunk) return; // prevent re-shrinking

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int newWidth = static_cast<int>(screenWidth * 0.9);
    int newHeight = static_cast<int>(screenHeight * 0.9);
    int centerX = (screenWidth - newWidth) / 2;
    int centerY = (screenHeight - newHeight) / 2;
    RECT newWorkArea = { centerX, centerY, centerX + newWidth, centerY + newHeight };
    SystemParametersInfo(SPI_SETWORKAREA, 0, &newWorkArea, SPIF_SENDCHANGE);
    isShrunk = true;
}

void ScreenManager::RestoreScreen()
{
    if (!isShrunk) return;
    SystemParametersInfo(SPI_SETWORKAREA, 0, &originalRect, SPIF_SENDCHANGE);
    isShrunk = false;
}
