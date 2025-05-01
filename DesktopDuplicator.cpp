#include "DesktopDuplicator.h"
#include <iostream>

DesktopDuplicator::DesktopDuplicator() {}
DesktopDuplicator::~DesktopDuplicator() { releaseFrame(); }

bool DesktopDuplicator::initialize(ComPtr<ID3D11Device> device) {
    ComPtr<IDXGIDevice> dxgiDevice;     if (FAILED(device.As(&dxgiDevice))) return false;
    ComPtr<IDXGIAdapter> adapter;       if (FAILED(dxgiDevice->GetAdapter(&adapter))) return false;
    ComPtr<IDXGIOutput> output;         if (FAILED(adapter->EnumOutputs(0, &output))) return false;
    ComPtr<IDXGIOutput1> output1;       if (FAILED(output.As(&output1))) return false;
    return SUCCEEDED(output1->DuplicateOutput(device.Get(), &m_duplication));
}

bool DesktopDuplicator::acquireFrame(ComPtr<ID3D11Texture2D>& outTexture) {
    DXGI_OUTDUPL_FRAME_INFO info;
    ComPtr<IDXGIResource> resource;
    HRESULT hr = m_duplication->AcquireNextFrame(16, &info, &resource);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) return false;
    if (FAILED(hr)) return false;
    resource.As(&outTexture);
    return true;
}

void DesktopDuplicator::releaseFrame() {
    if (m_duplication) m_duplication->ReleaseFrame();
}
