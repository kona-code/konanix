#pragma once
#include <windows.h>

// class that manages the custom start menu window
class menumanager {
public:
    menumanager(); // initializes member variables and creates the menu window
    ~menumanager();

    void initialize();
    void showMenu();
    void hideMenu();
    void updateMenu();
    bool isVisible() const; // returns current visibility state
    void shutdown(); // cleanup before exit

private:
    HWND menuWindow; // handle to the menu window
    bool visible;
};
