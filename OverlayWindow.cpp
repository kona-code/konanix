#include "OverlayWindow.h"
#include <stdexcept>

OverlayWindow::OverlayWindow(HINSTANCE h, int x, int y, int w, int hgt)
    : m_hInstance(h), m_x(x), m_y(y), m_width(w), m_height(hgt), m_hwnd(nullptr) {
}

OverlayWindow::~OverlayWindow() {
    if (m_hwnd) DestroyWindow(m_hwnd);
}

bool OverlayWindow::create() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = wndProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = L"Konanix Manipulative Overlay";
    RegisterClass(&wc);

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        wc.lpszClassName, nullptr,
        WS_POPUP,
        m_x, m_y, m_width, m_height,
        nullptr, nullptr, m_hInstance, nullptr
    );
    if (!m_hwnd) return false;

    HRGN rgn = CreateRoundRectRgn(0, 0, m_width, m_height, 16, 16);
    SetWindowRgn(m_hwnd, rgn, TRUE);
    return true;
}

void OverlayWindow::show() { ShowWindow(m_hwnd, SW_SHOW); }
void OverlayWindow::hide() { ShowWindow(m_hwnd, SW_HIDE); }
HWND OverlayWindow::handle() const { return m_hwnd; }

LRESULT CALLBACK OverlayWindow::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, msg, wp, lp);
}
