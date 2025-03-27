#include "hotkeyhandler.h"
#include "windowmanager.h" // Include the full definition

// Define static members
WindowManager* HotkeyHandler::s_pWindowManager = nullptr;
bool HotkeyHandler::isWindowsKeyPressed = false;

HotkeyHandler::HotkeyHandler(HINSTANCE hInst, WindowManager* windowManager)
    : hInstance(hInst)
{
    // Save the pointer in the static member
    s_pWindowManager = windowManager;
}

bool HotkeyHandler::Initialize() {
    if (SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0) == nullptr) {
        MessageBox(NULL, L"Failed to install global keyboard hook!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void HotkeyHandler::RegisterHotkey() {
    // Optional: Register additional hotkeys if needed.
}

LRESULT CALLBACK HotkeyHandler::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP) {
            if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
                if (wParam == WM_KEYDOWN && !isWindowsKeyPressed) {
                    isWindowsKeyPressed = true;
                    if (s_pWindowManager) {
                        s_pWindowManager->ToggleStartMenu(true); // Show custom start menu
                    }
                }
                else if (wParam == WM_KEYUP && isWindowsKeyPressed) {
                    isWindowsKeyPressed = false;
                    if (s_pWindowManager) {
                        s_pWindowManager->ToggleStartMenu(false); // Hide custom start menu
                    }
                }
                return 1;  // Block the default behavior.
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
