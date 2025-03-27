#include "windowmanager.h"
#include <windows.h>
#include "magnifiermanager.h"

MagnifierManager g_magnifierManager; // global or member variable

void WindowManager::ToggleStartMenu(bool isPressed) {
    if (isPressed) {
            if (!g_magnifierManager.Initialize(hInstance)) {
                MessageBox(NULL, L"Failed to initialize magnifier", L"Error", MB_OK | MB_ICONERROR);
                return;
            }

        g_magnifierManager.AnimateScale(0.8f, 100);  // Smooth zoom-in

        if (startMenuManager->IsVisible()) {
            startMenuManager->Hide();
            screenManager->RestoreScreen();
        }
        else {
            startMenuManager->Show();
            screenManager->ShrinkAndCenterScreen();
        }

    }
    else {
        g_magnifierManager.AnimateScale(1.0f, 80);

        // delay to allow animation to complete
        SetTimer(NULL, 2, 310, [](HWND, UINT, UINT_PTR, DWORD) {
            g_magnifierManager.Hide();
            KillTimer(NULL, 2);
            });
    }
}


/*void WindowManager::ToggleStartMenu(bool isPressed)
{
    // toggle the custom start menu and adjust the screen work area
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
}*/


WindowManager::WindowManager(HINSTANCE hInstance)
    : hInstance(hInstance), hwnd(nullptr), startMenuManager(nullptr),
    screenManager(nullptr), hotkeyHandler(nullptr)
{
    // empty constructor
}

WindowManager::~WindowManager()
{
    delete startMenuManager;
    delete screenManager;
    delete hotkeyHandler;
}

bool WindowManager::Initialize()
{
    // create start menu manager and screen manager
    startMenuManager = new StartMenuManager(hInstance, hwnd);
    screenManager = new ScreenManager();
    hotkeyHandler = new HotkeyHandler(hInstance, this);
    if (!hotkeyHandler->Initialize()) {
        MessageBox(NULL, L"failed to install hotkey hook", L"error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void WindowManager::Run()
{
    // main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void WindowManager::RegisterHotkey()
{
    hotkeyHandler->RegisterHotkey();
}


void WindowManager::Shutdown()
{
    startMenuManager->Shutdown();
    UnregisterHotKey(NULL, 1);
}
