#pragma once
#include <windows.h>

class OverlayWindow {
public:
    OverlayWindow(HINSTANCE hInst, int x, int y, int w, int h);
    ~OverlayWindow();

    bool create();
    void show();
    void hide();
    HWND handle() const;

private:
    HINSTANCE m_hInstance;
    int       m_x, m_y, m_width, m_height;
    HWND      m_hwnd;
    static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
};
