/*#include "dcompmanager.h"
#include <dcomp.h>
#include <d2d1.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

DCompManager::DCompManager() : m_hwnd(nullptr) {
    // empty constructor
}

DCompManager::~DCompManager() {
    // automatic release via ComPtr
}

HRESULT DCompManager::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
    HRESULT hr = DCompositionCreateDevice(
        nullptr,
        IID_PPV_ARGS(&m_device)
    );
    if (FAILED(hr)) return hr;

    hr = m_device->CreateTargetForHwnd(hwnd, TRUE, &m_target);
    if (FAILED(hr)) return hr;

    hr = m_device->CreateVisual(&m_rootVisual);
    if (FAILED(hr)) return hr;

    hr = m_target->SetRoot(m_rootVisual.Get());
    if (FAILED(hr)) return hr;

    m_device->Commit();
    return S_OK;
}

HRESULT DCompManager::ApplyScaleTransform(float scale) {
    // create a scaling transform matrix using Direct2D helper
    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Scale(scale, scale);
    HRESULT hr = m_rootVisual->SetTransform(matrix);
    Commit();
    return hr;
}

HRESULT DCompManager::RemoveScaleTransform() {
    D2D1_MATRIX_3X2_F identity = D2D1::Matrix3x2F::Identity();
    HRESULT hr = m_rootVisual->SetTransform(identity);
    Commit();
    return hr;
}

void DCompManager::Commit() {
    if (m_device) {
        m_device->Commit();
    }
}*/
