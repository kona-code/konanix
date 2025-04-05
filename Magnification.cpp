#include "Magnification.h"
#include <iostream>

#pragma comment(lib, "dcomp.lib")

DirectCompositionMagnifier::DirectCompositionMagnifier() {
    // empty constructor
}

DirectCompositionMagnifier::~DirectCompositionMagnifier() {
    // resources will be automatically released by ComPtr
}

bool DirectCompositionMagnifier::Initialize(HWND hwnd) {
    // create a directcomposition device
    HRESULT hr = DCompositionCreateDevice(nullptr, IID_PPV_ARGS(&m_dcompDevice));
    if (FAILED(hr)) {
        std::cerr << "failed to create dcomp device" << std::endl;
        return false;
    }

    // create a composition target for the given window
    hr = m_dcompDevice->CreateTargetForHwnd(hwnd, TRUE, &m_dcompTarget);
    if (FAILED(hr)) {
        std::cerr << "failed to create dcomp target" << std::endl;
        return false;
    }

    // create a visual that we will transform\n
    hr = m_dcompDevice->CreateVisual(&m_dcompVisual);
    if (FAILED(hr)) {
        std::cerr << "failed to create dcomp visual" << std::endl;
        return false;
    }

    // set the visual as the root for the target\n
    hr = m_dcompTarget->SetRoot(m_dcompVisual.Get());
    if (FAILED(hr)) {
        std::cerr << "failed to set dcomp visual as root" << std::endl;
        return false;
    }

    // commit the initial state\n
    m_dcompDevice->Commit();
    return true;
}

bool DirectCompositionMagnifier::ApplyScale(float scale) {
    if (!m_dcompVisual) return false;

    // create a scaling matrix using D2D1 helper\n
    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Scale(scale, scale);
    HRESULT hr = m_dcompVisual->SetTransform(matrix);
    if (FAILED(hr)) {
        std::cerr << "failed to set transform" << std::endl;
        return false;
    }
    return true;
}

void DirectCompositionMagnifier::Commit() {
    if (m_dcompDevice) {
        m_dcompDevice->Commit();
    }
}
