#pragma once

#include <windows.h>
#include "startmenu.h"
#include "screenmanager.h"
#include "hotkeyhandler.h"

// class that manages the custom start menu and screen manipulation
class WindowManager {
public:
    WindowManager(HINSTANCE hInstance);
    ~WindowManager();
    bool Initialize();
    void Run();
    void RegisterHotkey();
    void ToggleStartMenu(bool isPressed);
    void Shutdown();

private:
    HINSTANCE hInstance;
    HWND hwnd; // main window handle (unused in this demo)
    StartMenuManager* startMenuManager;
    ScreenManager* screenManager;
    HotkeyHandler* hotkeyHandler;
};
