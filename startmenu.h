#pragma once
#include <windows.h>

// class that manages the custom start menu window
class StartMenuManager {
public:
    StartMenuManager(HINSTANCE hInstance, HWND hwndParent);
    ~StartMenuManager();
    void Show();
    void Hide();
    bool IsVisible() const;
    void Shutdown();

private:
    HINSTANCE hInstance;
    HWND hwndParent;
    HWND hwndStartMenu;
    bool isVisible;
};
