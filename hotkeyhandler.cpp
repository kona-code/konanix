#include "hotkeyhandler.h"
#include "windowmanager.h"

// define the static members
WindowManager* HotkeyHandler::s_pWindowManager = nullptr;
bool HotkeyHandler::isWindowsKeyPressed = false;
bool HotkeyHandler::isActive = false;

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
}

LRESULT CALLBACK HotkeyHandler::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
            if (wParam == WM_KEYDOWN && !isWindowsKeyPressed) {
                isWindowsKeyPressed = true;
                if (HotkeyHandler::s_pWindowManager && !isActive) {
					isActive = true;
                    HotkeyHandler::s_pWindowManager->ToggleStartMenu(true);
                }
                else if(HotkeyHandler::s_pWindowManager && isActive) {
					isActive = false;
                    HotkeyHandler::s_pWindowManager->ToggleStartMenu(false);
                }
            } 
            else if (wParam == WM_KEYUP) {
                isWindowsKeyPressed = false;

            }
            return 1; // block windows key default behavior
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

