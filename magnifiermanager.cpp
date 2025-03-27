#include "MagnifierManager.h"
#include <magnification.h>
#include <cmath>
#include <thread>
#include <chrono>

#pragma comment(lib, "Magnification.lib")

#define TIMER_ID 1
#define TIMER_INTERVAL 10 // 10ms per frame

MagnifierManager::MagnifierManager()
    : m_hwndMagnifier(nullptr), m_hwndOverlay(nullptr), m_initialized(false),
    m_currentScale(1.0f), m_targetScale(1.0f), m_animationSteps(0), m_currentStep(0) {
}

MagnifierManager::~MagnifierManager() {
    if (m_hwndMagnifier) DestroyWindow(m_hwndMagnifier);
    if (m_hwndOverlay) DestroyWindow(m_hwndOverlay);
    MagUninitialize();
}

bool MagnifierManager::IsInitialized() const {
    return m_initialized;
}

bool MagnifierManager::Initialize(HINSTANCE hInstance) {
    if (!MagInitialize()) return false;

    m_hwndOverlay = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, L"STATIC", L"",
        WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, hInstance, nullptr);
    if (!m_hwndOverlay) return false;

    SetLayeredWindowAttributes(m_hwndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(m_hwndOverlay, SW_SHOW);

    m_hwndMagnifier = CreateWindow(WC_MAGNIFIER, L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        m_hwndOverlay, nullptr, hInstance, nullptr);
    if (!m_hwndMagnifier) return false;

    // Store the instance pointer for the TimerProc callback
    SetWindowLongPtr(m_hwndOverlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    m_initialized = true;
    return true;
}

bool MagnifierManager::ApplyScaleTransform(float scale) {
    if (!m_initialized || !m_hwndMagnifier) return false;

    MAGTRANSFORM matrix = {};
    matrix.v[0][0] = scale; // Scale X
    matrix.v[1][1] = scale; // Scale Y
    matrix.v[2][2] = 1.0f;  // Must be 1 for proper transformation

    if (!MagSetWindowTransform(m_hwndMagnifier, &matrix)) return false;

    MagSetWindowSource(m_hwndMagnifier, { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) });

    m_currentScale = scale;
    return true;
}

bool MagnifierManager::RemoveScaleTransform() {
    return ApplyScaleTransform(1.0f);
}

void MagnifierManager::Show() {
    if (m_hwndOverlay) ShowWindow(m_hwndOverlay, SW_SHOW);
}

void MagnifierManager::Hide() {
    if (m_hwndOverlay) ShowWindow(m_hwndOverlay, SW_HIDE);
}

void MagnifierManager::AnimateScale(float targetScale, int durationMs) {
    if (!m_initialized || !m_hwndMagnifier) return;

    std::thread([this, targetScale, durationMs]() {
        const int steps = durationMs / TIMER_INTERVAL;
        for (int i = 0; i <= steps; i++) {
            float t = static_cast<float>(i) / steps;
            float newScale = m_currentScale * (1.0f - t) + targetScale * t;
            ApplyScaleTransform(newScale);
            std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVAL));
        }
        }).detach();
}
VOID CALLBACK MagnifierManager::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    MagnifierManager* pThis = reinterpret_cast<MagnifierManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pThis) pThis->StepScale();
}

void MagnifierManager::StepScale() {
    if (m_currentStep >= m_animationSteps) {
        KillTimer(m_hwndOverlay, TIMER_ID);
        ApplyScaleTransform(m_targetScale);
        return;
    }

    // Use proper interpolation
    float t = static_cast<float>(m_currentStep) / m_animationSteps;
    float newScale = m_currentScale * (1.0f - t) + m_targetScale * t;

    ApplyScaleTransform(newScale);
    m_currentStep++;
}
