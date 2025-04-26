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

// initialize all core functions
bool konanix::initialize()
{
	pKonanixInstance = this; // set pointer for keyboard hook callback

    // create modules
    menuManager = new menumanager();
    taskbarManager = new taskbarmanager();
    explorerIntegration = new explorerintegration();
    performanceMonitor = new performancemonitor();
    settingsManager = new Settings();
    themeManager = new thememanager();
    hotkeyHandler = new hotkeyhandler(hInstance, this);
    mController = new ManipulationController(hInstance);

    // Load hotkey handler
    if (!hotkeyHandler->initialize()) {
        MessageBox(NULL, L"Failed to install hotkey hook", L"Konanix - Initialization Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Initialize ManipulationController
    ManipulationController app(hInstance);
    if (!app.run()) return false;
    app.InitializeManipulation();
    mController->m_overlay->create();
    mController->m_overlay->show();

    // Initialize Vulkan renderer
    m_renderer = std::make_unique<VulkanRenderer>(mController->m_overlay->handle());
    try {
        if (!m_renderer->initialize()) {
            MessageBox(NULL, L"Failed to initialize VulkanRenderer.", L"Konanix - Initialization Error", MB_OK | MB_ICONERROR);
            return false;
        }
    }
    catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Konanix - Initialization Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // load settings and theme (unfinished functions)
    settingsManager->load();
    themeManager->applytheme("default");

    return true;
}

// run main message loop
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
	UnregisterHotKey(NULL, 1); // unregister hotkey (WINKEY/Super)
}

void konanix::registerHotkey()
{
    // additional registration
}

void konanix::toggleStartMenu(bool pressed) {
    if (!pressed) {
        if (!toggled) {
			//taskbarManager->hideTaskbar(); //uncomment to hide taskbar
			winStart->LoadApps();

            // simulate screen shrinking
            m_renderer->render(0.8f);

			//menuManager->showMenu(); //uncomment to enable custom start menu (unstable and unfinished)
            toggled = !toggled;
        }
        else {
			//taskbarManager->showTaskbar(); //uncomment to restore taskbar (enable if "hideTaskbar() is uncommented")
            //menuManager->hideMenu();

            // simulate screen restoring
            m_renderer->render(1.0f);

            // restore the screen
            //magnifier->RemoveScaleTransform();
			//magnifier->Hide(); // enable user input - hides m_hwndOverlay window
            toggled = !toggled;

        }
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
