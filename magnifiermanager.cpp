#include "MagnifierManager.h"
#include <magnification.h>

#pragma comment(lib, "Magnification.lib")

MagnifierManager::MagnifierManager()
    : m_hwndMagnifier(nullptr), m_hwndOverlay(nullptr), m_initialized(false) {
}

MagnifierManager::~MagnifierManager() {
    if (m_hwndMagnifier) {
        DestroyWindow(m_hwndMagnifier);
    }
    if (m_hwndOverlay) {
        DestroyWindow(m_hwndOverlay);
    }
    MagUninitialize();
}

bool MagnifierManager::Initialize(HINSTANCE hInstance) {
    if (!MagInitialize()) {
        return false;
    }

    // Create full-screen overlay window
    m_hwndOverlay = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, L"STATIC", L"",
        WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, hInstance, nullptr);

    if (!m_hwndOverlay) {
        return false;
    }

    SetLayeredWindowAttributes(m_hwndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(m_hwndOverlay, SW_SHOW);

    // Create magnifier control as a child of the overlay
    m_hwndMagnifier = CreateWindow(WC_MAGNIFIER, L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        m_hwndOverlay, nullptr, hInstance, nullptr);

    if (!m_hwndMagnifier) {
        return false;
    }

    m_initialized = true;
    return true;
}

bool MagnifierManager::ApplyScaleTransform(float scale) {
    if (!m_initialized || !m_hwndMagnifier) return false;

    MAGTRANSFORM matrix = { 0 };
    matrix.v[0][0] = scale;  // Scale X
    matrix.v[1][1] = scale;  // Scale Y
    matrix.v[2][2] = 1.0f;   // Must be 1 for proper transformation

    if (!MagSetWindowTransform(m_hwndMagnifier, &matrix)) {
        return false;
    }

    MagSetWindowSource(m_hwndMagnifier, { 0, 0, (LONG)GetSystemMetrics(SM_CXSCREEN), (LONG)GetSystemMetrics(SM_CYSCREEN) });
    return true;
}

bool MagnifierManager::RemoveScaleTransform() {
    if (!m_initialized || !m_hwndMagnifier) return false;

    MAGTRANSFORM identity = { 0 };
    identity.v[0][0] = 1.0f;
    identity.v[1][1] = 1.0f;
    identity.v[2][2] = 1.0f;

    if (!MagSetWindowTransform(m_hwndMagnifier, &identity)) {
        return false;
    }
    return true;
}

void MagnifierManager::Show() {
    if (m_hwndOverlay) {
        ShowWindow(m_hwndOverlay, SW_SHOW);
    }
}

void MagnifierManager::Hide() {
    if (m_hwndOverlay) {
        ShowWindow(m_hwndOverlay, SW_HIDE);
    }
}
