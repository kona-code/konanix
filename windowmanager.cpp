#include "windowmanager.h"
#include <windows.h>
#include <iostream>

WindowManager::WindowManager(HINSTANCE hInstance)
    : hInstance(hInstance), hwnd(nullptr), startMenuManager(nullptr),
    screenManager(nullptr), hotkeyHandler(nullptr) {
    // empty constructor
}

WindowManager::~WindowManager() {
    delete startMenuManager;
    delete screenManager;
    delete hotkeyHandler;
}

bool WindowManager::Initialize() {
    // initialize start menu manager and screen manager
    startMenuManager = new StartMenuManager(hInstance, hwnd);
        screenManager = new ScreenManager();
        // create a hotkey handler and pass this object
    hotkeyHandler = new HotkeyHandler(hInstance, this);
        if (!hotkeyHandler->Initialize()) {
            MessageBox(NULL, L"failed to install hotkey hook", L"error", MB_OK | MB_ICONERROR);
                return false;
        };
    return true;
}

void WindowManager::Run() {
    // main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
            DispatchMessage(&msg);
    }
}

void WindowManager::RegisterHotkey() {
    // call register hotkey in hotkey handler if needed
    hotkeyHandler->RegisterHotkey();
}

void WindowManager::ToggleStartMenu(bool isPressed) {
    // toggle the custom start menu and adjust the screen
    if (isPressed) {
        if (startMenuManager->IsVisible()) {
            startMenuManager->Hide();
            screenManager->RestoreScreen();
        }
        else {
            startMenuManager->Show();
                screenManager->ShrinkAndCenterScreen();
        }
    }
}

void WindowManager::Shutdown() {
    // clean up and unregister hotkey
    startMenuManager->Shutdown();
        UnregisterHotKey(NULL, 1);
}
