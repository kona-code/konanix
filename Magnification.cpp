#include "Magnification.h"

#define TIMER_INTERVAL 10 // in milliseconds

MagnifierManager::MagnifierManager()
    : m_hwndMagnifier(nullptr), m_hwndOverlay(nullptr), m_initialized(false),
    m_currentScale(1.0f), m_targetScale(1.0f), m_animationSteps(0), m_currentStep(0),
    m_refreshing(false)
{
    // Enable DPI awareness (ideally, you would call SetProcessDPIAware() once at program start)
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
        std::cerr << "failed to initialize magnification API" << std::endl;
        return false;
    }

    // Create a full-screen overlay window.
    // (If you want to capture only the work area, use SPI_GETWORKAREA here.)
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
    // remove from taskbar
    SetWindowLong(m_hwndOverlay, GWL_EXSTYLE, GetWindowLong(m_hwndOverlay, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

    // Instead of using a simple color key, update the layered window with a custom image.
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Load the custom background image (BMP). Adjust the file path as needed.
    HBITMAP hCustomImage = (HBITMAP)LoadImageW(
        NULL,
        L"C:\\miku.bmp",
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION
    );
    if (hCustomImage) {
        BITMAP bm = { 0 };
        GetObject(hCustomImage, sizeof(bm), &bm);

        SIZE sizeWin = { m_screenWidth, m_screenHeight };
        POINT ptSrc = { 0, 0 };
        // Prepare the blend function for full opacity.
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
        // Optionally, keep hCustomImage if you want to refresh the background later;
        // otherwise, you can call DeleteObject(hCustomImage) here.
    }
    else {
        std::cerr << "failed to load custom background image" << std::endl;
    }
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcMem);
    // End custom background image update

    // Create the magnifier control as a child of the overlay.
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

    // Store instance pointer for callback use
    SetWindowLongPtr(m_hwndOverlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    Hide(); // start hidden
    m_initialized = true;
    return true;
}

bool MagnifierManager::ApplyScaleTransform(float scale) {
    if (!m_initialized || !m_hwndMagnifier)
        return false;

    float centerX = m_screenWidth / 2.0f;
    float centerY = m_screenHeight / 2.0f;

    // Compute the transformation matrix for scaling and centered translation.
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

    // Animate scaling in a detached thread.
    std::thread([this, startScale, targetScale]() {
        const int steps = m_animationSteps;
        for (int i = 0; i <= steps; i++) {
            float t = static_cast<float>(i) / steps;
            float newScale = startScale * (1.0f - t) + targetScale * t;
            ApplyScaleTransform(newScale);
            std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVAL));
        }
        // After animation, start a continuous refresh to keep the desktop updated.
        StartRefreshing();
        }).detach();
}

void MagnifierManager::Refresh() {
    // Set the source rectangle for the magnifier.
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
