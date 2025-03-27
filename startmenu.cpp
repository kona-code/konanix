#include "startmenu.h"
#include <windows.h>

StartMenuManager::StartMenuManager(HINSTANCE hInstance, HWND hwndParent)
    : hInstance(hInstance), hwndParent(hwndParent), hwndStartMenu(nullptr), isVisible(false)
{
    // empty constructor
}

StartMenuManager::~StartMenuManager()
{
    if (hwndStartMenu) {
        DestroyWindow(hwndStartMenu);
    }
}

void StartMenuManager::Show()
{
    if (isVisible) return;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int menuWidth = 1280;
    int menuHeight = 128;
    int posX = (screenWidth - menuWidth) / 2;
    int posY = screenHeight - menuHeight - 48; // px above bottom
    hwndStartMenu = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"start menu",
        WS_POPUP | WS_BORDER, posX, posY, menuWidth, menuHeight, hwndParent, nullptr, hInstance, nullptr);
    if (hwndStartMenu) {
        SetLayeredWindowAttributes(hwndStartMenu, RGB(0, 0, 0), 180, LWA_COLORKEY);
        ShowWindow(hwndStartMenu, SW_SHOW);
        SetForegroundWindow(hwndStartMenu);
        isVisible = true;
    }
}

void StartMenuManager::Hide()
{
    if (!isVisible) return;
    DestroyWindow(hwndStartMenu);
    hwndStartMenu = nullptr;
    isVisible = false;
}

bool StartMenuManager::IsVisible() const
{
    return isVisible;
}

void StartMenuManager::Shutdown()
{
    if (isVisible) {
        Hide();
    }
}
