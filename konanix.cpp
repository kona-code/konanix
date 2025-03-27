#include "konanix.h"
#include <iostream>

// Initialize static pointer
Konanix* Konanix::pKonanixInstance = nullptr;

Konanix::Konanix(HINSTANCE hInst)
    : hInstance(hInst), hwnd(nullptr), isWindowsKeyPressed(false),
    isStartMenuVisible(false), startMenuWnd(nullptr) {
}

Konanix::~Konanix() {
    if (startMenuWnd) {
        DestroyWindow(startMenuWnd);
    }
}

bool Konanix::Initialize() {
    pKonanixInstance = this;

    // Register a global keyboard hook
    if (SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0) == nullptr) {
        MessageBox(NULL, L"Failed to install global keyboard hook!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void Konanix::Run() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Konanix::ToggleStartMenu(bool pressed) {
    if (pressed) {
        if (!isWindowsKeyPressed) {
            isWindowsKeyPressed = true;

            if (isStartMenuVisible) {
                CloseStartMenu();
                screenManager.RestoreScreen();
                isStartMenuVisible = false;
            }
            else {
                ManipulateExplorerMenu(true);
                screenManager.ShrinkAndCenterScreen();
                CreateStartMenu();
                isStartMenuVisible = true;
            }
        }
    }
    else {
        isWindowsKeyPressed = false;
        ManipulateExplorerMenu(false);  // Ensure taskbar is restored
    }
}

void Konanix::CreateStartMenu() {
    startMenuWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, L"STATIC", L"Start Menu",
        WS_POPUP | WS_BORDER, 100, 100, 500, 400, hwnd, nullptr, hInstance, nullptr);
    if (!startMenuWnd) {
        MessageBox(hwnd, L"Failed to create start menu!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    SetLayeredWindowAttributes(startMenuWnd, RGB(0, 0, 0), 180, LWA_COLORKEY);

    ShowWindow(startMenuWnd, SW_SHOW);
    SetForegroundWindow(startMenuWnd);
}

void Konanix::CloseStartMenu() {
    if (startMenuWnd) {
        DestroyWindow(startMenuWnd);
        startMenuWnd = nullptr;
    }
}

void Konanix::ManipulateExplorerMenu(bool hide) {
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        ShowWindow(taskbar, hide ? SW_HIDE : SW_SHOW);
        UpdateWindow(taskbar);
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

        if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
            if (pKonanixInstance) {
                pKonanixInstance->ToggleStartMenu(wParam == WM_KEYDOWN);
            }

            // Allow key to pass if released, block otherwise
            if (wParam == WM_KEYDOWN) {
                return 1;  // Block Windows Key behavior
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
