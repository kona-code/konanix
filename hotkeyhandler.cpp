#include "hotkeyhandler.h"
#include "windowmanager.h"
#include <iostream>

// define the static members
WindowManager* HotkeyHandler::s_pWindowManager = nullptr;
HHOOK HotkeyHandler::s_hHook = nullptr;
bool HotkeyHandler::isWindowsKeyPressed = false;
bool HotkeyHandler::isActive = false;

HotkeyHandler::HotkeyHandler(HINSTANCE hInst, WindowManager* windowManager)
    : hInstance(hInst)
{
    s_pWindowManager = windowManager;
}

bool HotkeyHandler::Initialize() {
    s_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    if (!s_hHook) {
        MessageBox(NULL, L"Failed to install global keyboard hook", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void HotkeyHandler::Unhook() {
    if (s_hHook) {
        UnhookWindowsHookEx(s_hHook);
        s_hHook = nullptr;
    }
}

void ReleaseWindowsKey() {
    // Create an INPUT structure to send a fake key release
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_LWIN;  // Left Windows Key
    input.ki.dwFlags = KEYEVENTF_KEYUP;  // Release the key

    // Send the event
    SendInput(1, &input, sizeof(INPUT));

    input.ki.wVk = VK_RWIN;  // Right Windows Key (optional)
    SendInput(1, &input, sizeof(INPUT));
}


LRESULT CALLBACK HotkeyHandler::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        // Check if it's a Windows key (Left or Right)
        if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {

            if (wParam == WM_KEYDOWN) {
                // If the Windows key is pressed, block its default action (Start menu)
                if (!isWindowsKeyPressed) {
                    isWindowsKeyPressed = true;  // Set flag to indicate key is pressed
                    if (HotkeyHandler::s_pWindowManager && !isActive) {
                        isActive = true;
                        HotkeyHandler::s_pWindowManager->ToggleStartMenu(true);  // Activate custom behavior
                    }
                    else if (HotkeyHandler::s_pWindowManager && isActive) {
                        isActive = false;
                        HotkeyHandler::s_pWindowManager->ToggleStartMenu(false);  // Deactivate custom behavior
                    }

                    return 1; // Block default Windows key behavior (Start menu)
                }
            }
            else if (wParam == WM_KEYUP) {
                // If the Windows key is released, we prevent it from doing anything.
                if (isWindowsKeyPressed) {
                    isWindowsKeyPressed = false; // Reset flag for key release

                    // Simulate the release of the Windows key to inform Windows
                    INPUT input = { 0 };
                    input.type = INPUT_KEYBOARD;
                    input.ki.wVk = keyStruct->vkCode;  // Windows Key (Left or Right)
                    input.ki.dwFlags = KEYEVENTF_KEYUP;  // Release the key
                    SendInput(1, &input, sizeof(INPUT));

                    // Block the default Windows key behavior
                    return 1; // Prevent any default behavior from occurring
                }
            }
        }
    }

    // Allow all other keys to pass through
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
