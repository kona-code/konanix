#pragma once
#include <windows.h>

// manages the taskbar (hiding and showing it)
class taskbarmanager {
public:
    taskbarmanager();
    ~taskbarmanager();

    void hideTaskbar();
    void showTaskbar();
    void updateTaskbar();
    bool isVisible() const; // returns current visibility state

private:
    HWND taskbarWindow; // handle to the taskbar window
    bool visible;
};
