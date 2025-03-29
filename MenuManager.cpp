#include "menumanager.h"
#include <windows.h>

// create a layered window for the start menu
menumanager::menumanager() : menuWindow(nullptr), visible(false) {
    menuWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"custom start menu",
        WS_POPUP | WS_BORDER, 0, 0, 800, 200, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
    if (menuWindow) {
        // set window transparency with alpha (max 255)
        SetLayeredWindowAttributes(menuWindow, RGB(0, 0, 0), 180, LWA_ALPHA);
    }
}

// destroy the menu window
menumanager::~menumanager() {
    if (menuWindow) {
        DestroyWindow(menuWindow);
        menuWindow = nullptr;
    }
}


void menumanager::initialize() {
	// will add later if needed
}

// position and display the menu window at bottom center
void menumanager::showMenu() {
    if (!menuWindow) return;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int menuWidth = 800;
    int menuHeight = 200;
    int posX = (screenWidth - menuWidth) / 2;
    int posY = screenHeight - menuHeight - 50; // 50 pixels above bottom
    SetWindowPos(menuWindow, HWND_TOPMOST, posX, posY, menuWidth, menuHeight, SWP_SHOWWINDOW);
    visible = true;
}

void menumanager::hideMenu() {
    if (menuWindow) {
        ShowWindow(menuWindow, SW_HIDE);
        visible = false;
    }
}

// update menu content dynamically
void menumanager::updateMenu() {
	// will add later
}

bool menumanager::isVisible() const {
    return visible;
}

void menumanager::shutdown() {
    if (visible) hideMenu();
}
