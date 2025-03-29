#pragma once
#include <windows.h>

class konanix;  // forward declaration of main class

// handles global keyboard hook for the windows key
class hotkeyhandler {

public:
    hotkeyhandler(HINSTANCE hInstance, konanix* windowManager);
    bool initialize();
    void unhook();
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
private:
    HINSTANCE hInstance;
    static konanix* s_pWindowManager;
    static bool isWindowsKeyPressed;
    static HHOOK s_hHook;
};