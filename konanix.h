#pragma once

#include <windows.h>
#include <string>
#include "screenmanager.h"

class Konanix {
public:
    explicit Konanix(HINSTANCE hInst);
    ~Konanix();

    bool Initialize();
    void Run();

private:
    HINSTANCE hInstance;
    HWND hwnd;
    bool isWindowsKeyPressed;
    bool isStartMenuVisible;
    HWND startMenuWnd;
    ScreenManager screenManager;

    void RegisterHotkey();
    void ToggleStartMenu(bool pressed);
    void CreateStartMenu();
    void ManipulateExplorerMenu(bool hide);
    void CloseStartMenu();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static Konanix* pKonanixInstance;
};
