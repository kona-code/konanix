#include "menumanager.h"
#include "TaskbarRipper.h"
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <vector>

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        // use HIWORD(wParam) to check notification code
        if (LOWORD(wParam) >= 1000) {
            int index = LOWORD(wParam) - 1000;
            // Launch the application associated with the pinned shortcut index 'index'
            // You can retrieve this mapping from the enumeration code stored elsewhere
            ShellExecute(hwnd, L"open", L"yourapp.exe", nullptr, nullptr, SW_SHOWNORMAL);
        }
        break;
        // handle other messages...
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// create a layered window for the start menu
menumanager::menumanager() : menuWindow(nullptr), visible(false) {
    LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    menuWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"start menu",
        WS_POPUP | WS_BORDER, 0, 0, 1200, 180, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
    if (menuWindow) {
        // set window transparency with alpha (max 255)
        SetLayeredWindowAttributes(menuWindow, RGB(16, 18, 25), 160, LWA_ALPHA);
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
    // additional initialization if needed
}

// position and display the menu window at bottom center
void menumanager::showMenu() {
    if (!menuWindow) return;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - width) / 2;
    int posY = screenHeight - height - 50; // 50 pixels above bottom
    SetWindowPos(menuWindow, HWND_TOPMOST, posX, posY, width, height, SWP_SHOWWINDOW);
    visible = true;
    updateMenu(); // update content when showing menu
}

void menumanager::hideMenu() {
    if (menuWindow) {
        ShowWindow(menuWindow, SW_HIDE);
        visible = false;
    }
}

// update menu content dynamically by enumerating pinned shortcuts and creating clickable buttons
void menumanager::updateMenu() {
    if (!menuWindow) return;

    // remove any existing child windows (controls) from the menu window
    HWND child = nullptr;
    while ((child = FindWindowEx(menuWindow, nullptr, L"BUTTON", nullptr)) != NULL) {
        DestroyWindow(child);
    }

    std::vector<PinnedShortcut> shortcuts = GetPinnedTaskbarShortcuts();

    // layout parameters
    const int iconSize = 64;         // size for icon area
    const int textHeight = 20;       // height for the label text
    const int itemWidth = iconSize + 20; // width for each item (including margins)
    const int itemHeight = iconSize + textHeight + 10; // total height for each item
    const int spacing = 10;

    int numItems = static_cast<int>(shortcuts.size());
    if (numItems == 0) return;

    // Calculate total width needed
    int totalWidth = numItems * itemWidth + (numItems - 1) * spacing;
    RECT rc;
    GetClientRect(menuWindow, &rc);
    int startX = (rc.right - rc.left - totalWidth) / 2;
    int y = 10; // top margin

    for (int i = 0; i < numItems; i++) {
        int x = startX + i * (itemWidth + spacing);

        // get the icon using SHGetFileInfo
        SHFILEINFOW sfi = { 0 };
        if (SHGetFileInfoW(shortcuts[i].path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
            // create a button control for the pinned shortcut; we'll use BS_ICON style\n
            HWND hButton = CreateWindowExW(0, L"BUTTON", shortcuts[i].name.c_str(),
                WS_CHILD | WS_VISIBLE | BS_ICON | BS_CENTER,
                x, y, iconSize, iconSize, menuWindow, reinterpret_cast<HMENU>(1000 + i), GetModuleHandle(NULL), nullptr);
            if (hButton) {
                SendMessageW(hButton, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(sfi.hIcon));
            }
        }
        // create a separate static control for the shortcut name (if needed)\n
        HWND hText = CreateWindowExW(0, L"STATIC", shortcuts[i].name.c_str(),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            x, y + iconSize, iconSize, textHeight, menuWindow, nullptr, GetModuleHandle(NULL), nullptr);
    }
}

bool menumanager::isVisible() const {
    return visible;
}

void menumanager::shutdown() {
    if (visible) hideMenu();
}
