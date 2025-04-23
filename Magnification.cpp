#include "Magnification.h"

#define TIMER_INTERVAL 10 // in milliseconds\
// define a smaller number for longer, but smoother animation
// define a larger number for faster, but rougher animation

MagnifierManager::MagnifierManager()
    : m_hwndMagnifier(nullptr), m_hwndOverlay(nullptr), m_initialized(false),
    m_currentScale(1.0f), m_targetScale(1.0f), m_animationSteps(0), m_currentStep(0),
    m_refreshing(false)
{
    SetProcessDPIAware();// enable DPI awareness
    
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN); // get and set screen width
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN); // get and set screen height
}

// destruct
MagnifierManager::~MagnifierManager() {
    StopRefreshing();
    if (m_hwndMagnifier)
        DestroyWindow(m_hwndMagnifier);
    if (m_hwndOverlay)
        DestroyWindow(m_hwndOverlay);
    MagUninitialize();
}

// check if initialized
bool MagnifierManager::IsInitialized() const {
    return m_initialized;
}

// initialize magnifier
bool MagnifierManager::Initialize(HINSTANCE hInstance) {
    if (!MagInitialize()) {
        std::cerr << "failed to initialize magnification API" << std::endl;
        return false;
    }

    // create a full-screen overlay window
    m_hwndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        L"STATIC", L"Konanix Manipulative Overlay",
        WS_POPUP,
        0, 0, m_screenWidth, m_screenHeight,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!m_hwndOverlay) {
        std::cerr << "failed to create overlay window" << std::endl;
        return false;
    }

    // remove (hide) from taskbar
    SetWindowLong(m_hwndOverlay, GWL_EXSTYLE, GetWindowLong(m_hwndOverlay, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// set custom background image (BMP format) - currently not working
    HBITMAP hCustomImage = (HBITMAP)LoadImageW(
        NULL,
        L"C:\\img.bmp",
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION
    );
    if (hCustomImage) {
        BITMAP bm = { 0 };
        GetObject(hCustomImage, sizeof(bm), &bm);

        SIZE sizeWin = { m_screenWidth, m_screenHeight };
        POINT ptSrc = { 0, 0 };
        // prepare the blend function for full opacity
        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = 0;  // BMPs usually don't have an alpha channel

        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hCustomImage);
        if (!UpdateLayeredWindow(m_hwndOverlay, hdcScreen, NULL, &sizeWin, hdcMem,
            &ptSrc, 0, &blend, ULW_ALPHA)) {
            std::cerr << "failed to update layered window with custom image" << std::endl;
        }
        SelectObject(hdcMem, hOldBmp);
    }
    else {
        std::cerr << "failed to load custom background image" << std::endl;
    }
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcMem);
    // end custom background image update

    // create the magnifier control as a child of the overlay
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

    // store instance pointer for callback use
    SetWindowLongPtr(m_hwndOverlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    HWND hLabel = CreateWindowExW(
        0,
        L"STATIC",
        L"Overlay Label",
        WS_CHILD | WS_EX_TOPMOST | WS_VISIBLE,
        20, 50, 200, 20,
        m_hwndOverlay,
        nullptr,
        hInstance,
        nullptr
    );


	Hide(); // hide the overlay window initially - do not remove, otherwise user input will be blocked
    m_initialized = true;
    return true;
}

bool MagnifierManager::ApplyScaleTransform(float scale) {
    if (!m_initialized || !m_hwndMagnifier)
        return false;

    float centerX = m_screenWidth / 2.0f;
    float centerY = m_screenHeight / 2.0f;

    // compute the transformation matrix for scaling and centered translation
    MAGTRANSFORM matrix = { 0 };
    matrix.v[0][0] = scale;
    matrix.v[0][2] = (1.0f - scale) * centerX;
    matrix.v[1][1] = scale;
    matrix.v[1][2] = (1.0f - scale) * centerY;
    matrix.v[2][2] = 1.0f;

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
        // start a continuous refresh/render to keep the desktop updated
        StartRefreshing();
        }).detach();
}

void MagnifierManager::Refresh() {
    // set source rectangle for the magnifier
    RECT src = { 0, 0, m_screenWidth, m_screenHeight };
    MagSetWindowSource(m_hwndMagnifier, src);
    UpdateWindow(m_hwndOverlay);
}

void MagnifierManager::StartRefreshing() {
    m_refreshing = true;
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
