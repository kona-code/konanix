#include "konanix.h"
#include <iostream>

// Initialize the static pointer to nullptr
Konanix* Konanix::pKonanixInstance = nullptr;

Konanix::Konanix(HINSTANCE hInst)
    : hInstance(hInst), hwnd(nullptr), isWindowsKeyPressed(false),
    isStartMenuVisible(false), startMenuWnd(nullptr) {
}

Konanix::~Konanix() {
    // Cleanup resources if necessary
    if (startMenuWnd) {
        DestroyWindow(startMenuWnd);
    }
}

bool Konanix::Initialize() {
    // Set the static instance to the current object
    pKonanixInstance = this;

    // Register a global keyboard hook
    if (SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0) == nullptr) {
        MessageBox(NULL, L"Failed to install global keyboard hook!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Register for the Windows Key (LWin/RWin) to toggle the Start Menu
    RegisterHotkey();
    return true;
}

void Konanix::Run() {
    // The main loop will not be required in this case since we are handling events with hooks
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Konanix::RegisterHotkey() {
    // Register the "Windows Key" press to toggle the Start Menu
    if (!RegisterHotKey(hwnd, 1, MOD_WIN, VK_LWIN)) {
        MessageBox(hwnd, L"Failed to register Windows key hotkey!", L"Error", MB_OK | MB_ICONERROR);
    }
}

void Konanix::ToggleStartMenu(bool pressed) {
    // If Windows key is pressed
    if (pressed && !isWindowsKeyPressed) {
        isWindowsKeyPressed = true;

        // Toggle start menu visibility
        if (isStartMenuVisible) {
            CloseStartMenu();  // Close the start menu
            screenManager.RestoreScreen();  // Restore the original screen size
            isStartMenuVisible = false;
        }
        else {
            ManipulateExplorerMenu(true);  // Hide the existing start menu
            screenManager.ShrinkAndCenterScreen();  // Shrink and center the screen
            CreateStartMenu();  // Show our custom start menu
            isStartMenuVisible = true;
        }
    }
    // If Windows key is released
    else if (!pressed && isWindowsKeyPressed) {
        isWindowsKeyPressed = false;
    }
}

void Konanix::CreateStartMenu() {
    // For simplicity, create a basic start menu window that is styled similarly to GNOME
    startMenuWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"Start Menu",
        WS_POPUP | WS_BORDER, 100, 100, 500, 400, hwnd, nullptr, hInstance, nullptr);
    if (startMenuWnd == nullptr) {
        MessageBox(hwnd, L"Failed to create start menu!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Make the window semi-transparent to mimic GNOME's aesthetic
    SetLayeredWindowAttributes(startMenuWnd, RGB(0, 0, 0), 180, LWA_COLORKEY);

    // Show the custom start menu
    ShowWindow(startMenuWnd, SW_SHOW);
    SetForegroundWindow(startMenuWnd);
}

void Konanix::CloseStartMenu() {
    if (startMenuWnd) {
        DestroyWindow(startMenuWnd);  // Close the start menu window
        startMenuWnd = nullptr;
    }
}

void Konanix::ManipulateExplorerMenu(bool hide) {
    // Hide or show the existing Windows Start menu (explorer.exe) by simulating a taskbar manipulation.
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        if (hide) {
            ShowWindow(taskbar, SW_HIDE);  // Hide the taskbar to simulate the start menu being replaced
        }
        else {
            ShowWindow(taskbar, SW_SHOW);  // Show it back
        }
    }
}

LRESULT CALLBACK Konanix::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK Konanix::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP) {
            if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
                // Check if the instance is valid before accessing it
                if (pKonanixInstance) {
                    pKonanixInstance->ToggleStartMenu(wParam == WM_KEYDOWN);
                }
                // Prevent default behavior of the Windows key
                return 1;  // This prevents the start menu from opening
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
