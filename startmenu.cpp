#include "startmenu.h"

StartMenuManager::StartMenuManager(HINSTANCE hInstance, HWND hwndParent)
    : hInstance(hInstance), hwndParent(hwndParent), isVisible(false), hwndStartMenu(nullptr) {
}

StartMenuManager::~StartMenuManager() {
    if (hwndStartMenu) {
        DestroyWindow(hwndStartMenu);
    }
}

void StartMenuManager::Show() {
    if (isVisible) return;

    hwndStartMenu = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"Start Menu",
        WS_POPUP | WS_BORDER, 100, 100, 500, 400, hwndParent, nullptr, hInstance, nullptr);
    if (hwndStartMenu) {
        SetLayeredWindowAttributes(hwndStartMenu, RGB(0, 0, 0), 180, LWA_COLORKEY);
        ShowWindow(hwndStartMenu, SW_SHOW);
        SetForegroundWindow(hwndStartMenu);
        isVisible = true;
    }
}

void StartMenuManager::Hide() {
    if (!isVisible) return;

    DestroyWindow(hwndStartMenu);
    hwndStartMenu = nullptr;
    isVisible = false;
}

bool StartMenuManager::IsVisible() const {
    return isVisible;
}

void StartMenuManager::Shutdown() {
    if (isVisible) {
        Hide();
    }
}
