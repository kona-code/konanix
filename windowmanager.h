#pragma once

#include <windows.h>
#include "startmenu.h"
#include "screenmanager.h"
#include "hotkeyhandler.h"
#include "dcompmanager.h"

// class that manages the custom start menu, overlay, and screen manipulation
class WindowManager {
public:
    WindowManager(HINSTANCE hInstance);
    ~WindowManager();
    bool Initialize();
    void Run();
    void RegisterHotkey();
    void ToggleStartMenu(bool isPressed);
    void Shutdown();
    // functions to control the overlay scaling
    void ActivateOverlay();
    void DeactivateOverlay();

private:
    HINSTANCE hInstance;
    HWND hwnd; // unused main window handle in this sample
    StartMenuManager* startMenuManager;
    ScreenManager* screenManager;
    HotkeyHandler* hotkeyHandler;
    //DCompManager* dcompManager;
    bool overlayActive;
};
