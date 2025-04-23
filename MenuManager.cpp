#include "menumanager.h"
#include "TaskbarRipper.h"
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <vector>

// create a top-level window for the start menu
menumanager::menumanager() : menuWindow(nullptr), visible(false) {

    menuWindow = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,  // layered (no taskbar button)
        L"STATIC", L"start menu",
        WS_POPUP,  // top-level window
        0, 0, width, height,
        NULL, nullptr, GetModuleHandle(NULL), nullptr
    );

    if (menuWindow) {
        // set window transparency
        SetLayeredWindowAttributes(menuWindow, RGB(16, 18, 25), 160, LWA_ALPHA);
    }
}

menumanager::~menumanager() {
    if (menuWindow) {
        DestroyWindow(menuWindow);
        menuWindow = nullptr;
    }
}

void menumanager::initialize() {
    // additional initialization if needed
}

void menumanager::showMenu() {
    if (!menuWindow)
        return;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - width) / 2;
	int posY = screenHeight - height - 50; // position above the taskbar (in px)

    // place the window at the top
    SetWindowPos(menuWindow, HWND_TOPMOST, posX, posY, width, height, SWP_SHOWWINDOW | SWP_NOACTIVATE);
    visible = true;
    updateMenu(); // update pinned shortcuts
}

void menumanager::hideMenu() {
    if (menuWindow) {
        ShowWindow(menuWindow, SW_HIDE);
        visible = false;
    }
}

// update menu content by enumerating pinned shortcuts and creating clickable controls
void menumanager::updateMenu() {
    if (!menuWindow)
        return;

    // remove any existing child controls
    HWND child = nullptr;
    while ((child = FindWindowEx(menuWindow, nullptr, L"BUTTON", nullptr)) != NULL) {
        DestroyWindow(child);
    }
    while ((child = FindWindowEx(menuWindow, nullptr, L"STATIC", nullptr)) != NULL) {
        DestroyWindow(child);
    }

    std::vector<PinnedShortcut> shortcuts = GetPinnedTaskbarShortcuts();
    if (shortcuts.empty()) return;

    // layout parameters
    const int iconSize = 64;
    const int textHeight = 0;
    const int itemWidth = iconSize + 20;
    const int spacing = 10;

    int numItems = static_cast<int>(shortcuts.size());
    int totalWidth = numItems * itemWidth + (numItems - 1) * spacing;
    RECT rc;
    GetClientRect(menuWindow, &rc);
    int startX = (rc.right - rc.left - totalWidth) / 2;
    int yIcon = 10;
    int yText = yIcon + iconSize;

    for (int i = 0; i < numItems; i++) {
        int x = startX + i * (itemWidth + spacing);

        // retrieve the icon using SHGetFileInfoW
        SHFILEINFOW sfi = { 0 };
        if (SHGetFileInfoW(shortcuts[i].path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
            // load button for the icon
            HWND hButton = CreateWindowExW(0, L"BUTTON", nullptr,
                WS_CHILD | WS_VISIBLE | BS_ICON | BS_CENTER,
                x, yIcon, iconSize, iconSize, menuWindow, reinterpret_cast<HMENU>(1000 + i), GetModuleHandle(NULL), nullptr);
            if (hButton) {
                SendMessageW(hButton, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(sfi.hIcon));
            }
        }
        // static control for the shortcut name below the icon
        HWND hText = CreateWindowExW(0, L"STATIC", shortcuts[i].name.c_str(),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            x, yText, iconSize, textHeight, menuWindow, nullptr, GetModuleHandle(NULL), nullptr);
    }
}

bool menumanager::isVisible() const {
    return visible;
}

void menumanager::shutdown() {
    if (visible)
        hideMenu();
}
