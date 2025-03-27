#pragma once

#include <windows.h>

class WindowManager;  // forward declaration

class HotkeyHandler {
public:
    HotkeyHandler(HINSTANCE hInstance, WindowManager* windowManager);
    bool Initialize();
    void Unhook(); // Cleanup function

    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE hInstance;
    static WindowManager* s_pWindowManager;
    static bool isWindowsKeyPressed;
    static bool isActive;
    static HHOOK s_hHook; // Store the hook handle
};
