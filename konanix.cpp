#include "konanix.h"
#include <iostream>

#include <thread>
#include <atomic>

konanix* konanix::pKonanixInstance = nullptr;

konanix::konanix(HINSTANCE hInst)
    : hInstance(hInst), hwnd(nullptr),
    menuManager(nullptr), taskbarManager(nullptr),
    explorerIntegration(nullptr), performanceMonitor(nullptr),
	settingsManager(nullptr), themeManager(nullptr), hotkeyHandler(nullptr), 
	mController(nullptr), m_renderer(nullptr), toggled(false), winStart(nullptr)
{
    Logger::getInstance().log("");
    Logger::getInstance().log(">------------------------------------------------[started konanix]------------------------------------------------<");
	Logger::getInstance().logInfo("konanix instance created with HINSTANCE: " + std::to_string(reinterpret_cast<uintptr_t>(hInst)));
    // empty constructor
}

konanix::~konanix()
{
	Logger::getInstance().logInfo("Destroying konanix instance...");
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

    // load hotkey handler
	Logger::getInstance().logInfo("Loading hotkey handler...");
    if (!hotkeyHandler->initialize()) {
		Logger::getInstance().logError("Failed to initialize hotkey handler.");
        MessageBox(NULL, L"Failed to install hotkey hook", L"Konanix - Initialization Error", MB_OK | MB_ICONERROR);
        return false;
    }
	Logger::getInstance().logInfo("Hotkey handler loaded successfully.");

    // initialize ManipulationController
	Logger::getInstance().logInfo("Initializing Manipulation Controller...");
    try {
        mController->InitializeManipulation();

        mController->m_overlay->create();
        mController->m_overlay->show();

        Logger::getInstance().logInfo("Manipulation Controller initialized successfully.");
    }
    catch (const std::exception ex) {Logger::getInstance().logError("Could not inialize Manupulation Controller: "+ std::string(ex.what())); }

    // initialize Vulkan renderer
	Logger::getInstance().logInfo("Initializing Vulkan Renderer...");
    m_renderer = std::make_unique<VulkanRenderer>(mController->m_overlay->handle());
    try {
        if (!m_renderer->initialize()) {
			Logger::getInstance().logError("Failed to initialize VulkanRenderer.");
            MessageBox(NULL, L"Failed to initialize VulkanRenderer.", L"Konanix - Initialization Error", MB_OK | MB_ICONERROR);
            return false;
        }
    }
    catch (const std::exception& ex) {
		Logger::getInstance().logError("VulkanRenderer initialization error: " + std::string(ex.what()));
        MessageBoxA(NULL, ex.what(), "Konanix - Initialization Error", MB_OK | MB_ICONERROR);
        return false;
    }
	Logger::getInstance().logInfo("Vulkan Renderer initialized successfully.");

    // load settings and theme (unfinished functions)
	Logger::getInstance().logInfo("Loading preferences...");
    settingsManager->load();
    themeManager->applytheme("default");
	Logger::getInstance().logInfo("Preferences loaded successfully.");

    return true;
}

// run main message loop
void konanix::run()
{
	Logger::getInstance().logInfo("Started main message loop.");
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void konanix::shutdown()
{
	Logger::getInstance().logInfo("Shutting down konanix...");
    menuManager->shutdown();
	UnregisterHotKey(NULL, 1); // unregister hotkey (WINKEY/Super)
}

void konanix::registerHotkey()
{
    // additional registration
}

/*void konanix::toggleStartMenu(bool pressed) {
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
}*/

std::atomic<bool> isRendering(false);

void konanix::toggleStartMenu(bool pressed) {
    if (!pressed) {
        if (!toggled) {
            winStart->LoadApps();

            if (!isRendering.exchange(true)) {
                std::thread([this]() {
                    try {
                        m_renderer->render(0.8f);
                    }
                    catch (const std::exception& ex) {
                        Logger::getInstance().logError("Rendering error: " + std::string(ex.what()));
                    }
                    isRendering = false;
                    }).detach();
            }

            toggled = true;
        }
        else {
            if (!isRendering.exchange(true)) {
                std::thread([this]() {
                    try {
                        m_renderer->render(1.0f);
                    }
                    catch (const std::exception& ex) {
                        Logger::getInstance().logError("Rendering error: " + std::string(ex.what()));
                    }
                    isRendering = false;
                    }).detach();
            }

            toggled = false;
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
