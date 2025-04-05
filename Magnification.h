#pragma once
#include <windows.h>
#include <dcomp.h>
#include <d2d1.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// handles a directcomposition-based magnification effect
class DirectCompositionMagnifier {
public:
    DirectCompositionMagnifier();
    ~DirectCompositionMagnifier();

    // initialize the dcomp device and create an overlay for the given window handle
    bool Initialize(HWND hwnd);

    // apply a scale transform to the overlay
    bool ApplyScale(float scale);

    // commit changes to update the display
    void Commit();

private:
    ComPtr<IDCompositionDevice> m_dcompDevice;
    ComPtr<IDCompositionTarget> m_dcompTarget;
    ComPtr<IDCompositionVisual> m_dcompVisual;
};
