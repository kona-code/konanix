#include "Magnification.h"

#define TIMER_INTERVAL 10 // in milliseconds

MagnifierManager::MagnifierManager()
    : m_hwndMagnifier(nullptr), m_hwndOverlay(nullptr), m_initialized(false),
    m_currentScale(1.0f), m_targetScale(1.0f), m_animationSteps(0), m_currentStep(0),
    m_refreshing(false) {

    SetProcessDPIAware();
    m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
}

MagnifierManager::~MagnifierManager() {
    StopRefreshing();
    if (m_hwndMagnifier)
        DestroyWindow(m_hwndMagnifier);
    if (m_hwndOverlay)
        DestroyWindow(m_hwndOverlay);
    MagUninitialize();
}

bool MagnifierManager::IsInitialized() const {
    return m_initialized;
}

bool MagnifierManager::Initialize(HINSTANCE hInstance) {
    if (!MagInitialize()) {
        std::cerr << "failed to initialize magnification api" << std::endl;
        return false;
    }

    // create a full-screen overlay window
    m_hwndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        L"STATIC", L"",
        WS_POPUP,
        0, 0, m_screenWidth, m_screenHeight,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!m_hwndOverlay) {
        std::cerr << "failed to create overlay window" << std::endl;
        return false;
    }
    SetWindowLong(m_hwndOverlay, GWL_EXSTYLE, GetWindowLong(m_hwndOverlay, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);  // remove from taskbar


    // make the overlay window transparent using color key transparency
    SetLayeredWindowAttributes(m_hwndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(m_hwndOverlay, SW_SHOW);

    m_hwndMagnifier = CreateWindow(
        WC_MAGNIFIER, L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, m_screenWidth, m_screenHeight,
        m_hwndOverlay, nullptr, hInstance, nullptr
    );
    if (!m_hwndMagnifier) {
        std::cerr << "failed to create magnifier control" << std::endl;
        return false;
    }
    
    // store this instance pointer for potential timer callbacks (if needed)
    SetWindowLongPtr(m_hwndOverlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    Hide(); // hide overlay by default
    m_initialized = true;
    return true;
}

bool MagnifierManager::ApplyScaleTransform(float scale) {
    if (!m_initialized || !m_hwndMagnifier)
        return false;

    float centerX = m_screenWidth / 2.0f;
    float centerY = m_screenHeight / 2.0f;

    // compute translation so that scaling remains centered
    MAGTRANSFORM matrix = { 0 };
    matrix.v[0][0] = scale;                     // scale x-axis
    matrix.v[0][2] = (1.0f - scale) * centerX;    // translate x-axis
    matrix.v[1][1] = scale;                     // scale y-axis
    matrix.v[1][2] = (1.0f - scale) * centerY;    // translate y-axis
    matrix.v[2][2] = 1.0f;                      // z-axis remains 1

    if (!MagSetWindowTransform(m_hwndMagnifier, &matrix)) {
        std::cerr << "failed to set window transform" << std::endl;
        return false;
    }
    m_currentScale = scale;
    return true;
}

void MagnifierManager::AnimateScale(float targetScale, int durationMs) {
    if (!m_initialized || !m_hwndMagnifier)
        return;
    Show();

    float startScale = m_currentScale; // capture starting scale
    m_targetScale = targetScale;
    m_animationSteps = durationMs / TIMER_INTERVAL;
    m_currentStep = 0;

    // animate scaling in a detached thread
    std::thread([this, startScale, targetScale]() {
        const int steps = m_animationSteps;
        for (int i = 0; i <= steps; i++) {
            float t = static_cast<float>(i) / steps;
            float newScale = startScale * (1.0f - t) + targetScale * t;
            ApplyScaleTransform(newScale);
            std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVAL));
        }
        // after animation, start continuous refreshing to keep the magnifier updated
        StartRefreshing();
        }).detach();
}

void MagnifierManager::Refresh() {
    // force the magnifier control to update its source
    RECT src = { 0, 0, m_screenWidth, m_screenHeight };
    MagSetWindowSource(m_hwndMagnifier, src);
    UpdateWindow(m_hwndOverlay);
}

void MagnifierManager::StartRefreshing() {
    m_refreshing = true;
    // start a detached thread that periodically refreshes the magnifier control
    std::thread([this]() {
        while (m_refreshing) {
            Refresh();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        }).detach();
}

void MagnifierManager::StopRefreshing() {
    m_refreshing = false;
}

void MagnifierManager::Show() {
    if (m_hwndOverlay) {
        ShowWindow(m_hwndOverlay, SW_SHOW);
        UpdateWindow(m_hwndOverlay);
    }
}

void MagnifierManager::Hide() {
    StopRefreshing();
    if (m_hwndOverlay) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ShowWindow(m_hwndOverlay, SW_HIDE);
    }
}
