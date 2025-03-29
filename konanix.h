#pragma once

#include <windows.h>
#include "menumanager.h"
#include "taskbarmanager.h"
#include "explorerintegration.h"
#include "performancemonitor.h"
#include "settings.h"
#include "thememanager.h"
#include "hotkeyhandler.h"
#include "TaskbarManager.h"


class konanix {
public:
    explicit konanix(HINSTANCE hInst);
    ~konanix();

    bool initialize();
    void run();
    void shutdown();
    void registerHotkey();
    void toggleStartMenu(bool pressed);

private:
    HINSTANCE hInstance;
    HWND hwnd; // main window handle
    menumanager* menuManager;
    taskbarmanager* taskbarManager;
    explorerintegration* explorerIntegration;
    performancemonitor* performanceMonitor;
    settingsmanager* settingsManager;
    thememanager* themeManager;
    hotkeyhandler* hotkeyHandler;

    // helper functions
    void createStartMenu();
    void closeStartMenu();
    void manipulateExplorerMenu(bool hide);

    // static pointer for keyboard hook callback
    static konanix* pKonanixInstance;
};
