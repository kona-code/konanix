#include "hotkeyhandler.h"
#include "windowmanager.h"  // include the full definition of WindowManager

// define the static members
WindowManager* HotkeyHandler::s_pWindowManager = nullptr;
bool HotkeyHandler::isWindowsKeyPressed = false;

HotkeyHandler::HotkeyHandler(HINSTANCE hInst, WindowManager* windowManager)
    : hInstance(hInst)
{
    s_pWindowManager = windowManager;
}

bool HotkeyHandler::Initialize() {
    // set the global keyboard hook
    if (SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0) == nullptr) {
        MessageBox(NULL, L"failed to install global keyboard hook", L"error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void HotkeyHandler::RegisterHotkey() {
    // optional: additional hotkey registration can be done here
}

LRESULT CALLBACK HotkeyHandler::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP) {
            if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
                if (wParam == WM_KEYDOWN && !isWindowsKeyPressed) {
                    isWindowsKeyPressed = true;
                    if (HotkeyHandler::s_pWindowManager) {
                        HotkeyHandler::s_pWindowManager->ToggleStartMenu(true);
                    }
                }
                else if (wParam == WM_KEYUP && isWindowsKeyPressed) {
                    isWindowsKeyPressed = false;
                    if (HotkeyHandler::s_pWindowManager) {
                        HotkeyHandler::s_pWindowManager->ToggleStartMenu(false);
                    }
                }
                return 1; // block default behavior
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
