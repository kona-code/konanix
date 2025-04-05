#include "konanix.h"
#include <iostream>

konanix* konanix::pKonanixInstance = nullptr;

konanix::konanix(HINSTANCE hInst)
    : hInstance(hInst), hwnd(nullptr),
    menuManager(nullptr), taskbarManager(nullptr),
    explorerIntegration(nullptr), performanceMonitor(nullptr),
    settingsManager(nullptr), themeManager(nullptr), hotkeyHandler(nullptr)
{
    // empty constructor
}

konanix::~konanix()
{
    delete menuManager;
    delete taskbarManager;
    delete explorerIntegration;
    delete performanceMonitor;
    delete settingsManager;
    delete themeManager;
    delete hotkeyHandler;
}

bool konanix::initialize()
{
    pKonanixInstance = this;

    // create modules
    menuManager = new menumanager();
    taskbarManager = new taskbarmanager();
    explorerIntegration = new explorerintegration();
    performanceMonitor = new performancemonitor();
    settingsManager = new settingsmanager();
    themeManager = new thememanager();
    hotkeyHandler = new hotkeyhandler(hInstance, this);
    magnifier = new MagnifierManager();

    // initialize
    if (!hotkeyHandler->initialize()) {
        MessageBox(NULL, L"Failed to install hotkey hook", L"Konanix - Runtime Error", MB_OK | MB_ICONERROR);
        return false;
    }

	HWND MagnificationHWND = nullptr;
    //if (!magnifier->Initialize(hInstance, L"C:\\image.png")) {
    if (!magnifier->Initialize(hInstance)) {
        MessageBox(NULL, L"Failed to initialize magnification", L"Konanix - Runtime Error", MB_OK | MB_ICONERROR);
    }
    // load settings and theme
    settingsManager->loadSettings();
    themeManager->applytheme("default");

    return true;
}

void konanix::run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void konanix::shutdown()
{
    menuManager->shutdown();
    UnregisterHotKey(NULL, 1);
}

void konanix::registerHotkey()
{
    // for additional registration
}

void konanix::toggleStartMenu(bool pressed) {
    if (pressed) {
        if (!menuManager->isVisible()) {
            taskbarManager->hideTaskbar();

            // simulate screen shrinking
            magnifier->AnimateScale(0.8F, 50);
            menuManager->showMenu();
            
        }
        else {
            menuManager->hideMenu();

			// simulate screen restoring
			magnifier->AnimateScale(1.0F, 50);
            taskbarManager->showTaskbar();

            // restore the screen
            //magnifier->RemoveScaleTransform();
			magnifier->Hide(); // currently cancels the animation
        }
    }
    else {
        taskbarManager->showTaskbar();
    }
}




void konanix::createStartMenu()
{
    menuManager->showMenu();
}

void konanix::closeStartMenu()
{
    menuManager->hideMenu();
}

void konanix::manipulateExplorerMenu(bool hide)
{
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        ShowWindow(taskbar, hide ? SW_HIDE : SW_SHOW);
        UpdateWindow(taskbar);
    }
}
