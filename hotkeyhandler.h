#pragma once

#include <windows.h>

class WindowManager;  // forward declaration

// class that handles the global keyboard hook for the windows key
class HotkeyHandler {
public:
    HotkeyHandler(HINSTANCE hInstance, WindowManager* windowManager);
    bool Initialize();
    void RegisterHotkey(); // optional: additional registration if needed
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE hInstance;
    // static pointer so that the static callback can access the WindowManager instance
    static WindowManager* s_pWindowManager;
    static bool isWindowsKeyPressed;
    static bool isActive;
};
