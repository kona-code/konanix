#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Magnification.h"
#include "Settings.h"

// manages the custom start menu window
class menumanager {
public:
    menumanager();
    ~menumanager();

    void initialize();
    void showMenu();
    void hideMenu();
    void updateMenu(); // updates the menu with pinned shortcuts
    bool isVisible() const;
    void shutdown(); // cleanup before exit

private:
    HWND menuWindow; // handle to the menu window
	Settings settings; // settings pointer
	//ManipulationController mController; // magnifier manager instance

	//HWND overlayHwnd = mController.GetOverlayWindow(); // handle to the overlay window

    bool visible;

    int height = settings.getMenuHeight();
    int width = settings.getMenuWidth();;
};
