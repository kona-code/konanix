#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DesktopDuplicator {
public:
	DesktopDuplicator();
	~DesktopDuplicator();

	bool initialize(ComPtr<ID3D11Device> device);
	bool acquireFrame(ComPtr<ID3D11Texture2D>& outTexture);
	void releaseFrame();
private:

	ComPtr<IDXGIOutputDuplication> m_duplication;
};