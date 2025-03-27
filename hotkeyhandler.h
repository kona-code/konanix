#pragma once

#include <windows.h>
#include "windowmanager.h"

class HotkeyHandler {
public:
    HotkeyHandler(HINSTANCE hInstance, WindowManager* windowManager);
    bool Initialize();
    void RegisterHotkey(); // additional hotkey registration
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE hInstance;
    // instance for static callback access
	static WindowManager* s_pWindowManager; // static to prevent unexpected behavior
    static bool isWindowsKeyPressed;
};
