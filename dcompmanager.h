/*#pragma once
#include <windows.h>
#include <dcomp.h>
#include <d2d1.h>
#include <wrl/client.h>

#pragma comment(lib, "dcomp.lib")

// class that manages a DirectComposition visual overlay
class DCompManager {
public:
    DCompManager();
    ~DCompManager();
    HRESULT Initialize(HWND hwnd);
    HRESULT ApplyScaleTransform(float scale); // apply a scaling transform
    HRESULT RemoveScaleTransform();           // remove transform (restore identity)
    void Commit();                            // commit composition changes

private:
    Microsoft::WRL::ComPtr<IDCompositionDevice> m_device;
    Microsoft::WRL::ComPtr<IDCompositionTarget> m_target;
    Microsoft::WRL::ComPtr<IDCompositionVisual> m_rootVisual;
    HWND m_hwnd;
};*/