#include "Controller.h"
#include <memory>
#include <d3d11.h>  
#include <wrl/client.h>  

#pragma comment(lib, "d3d11.lib")  


ManipulationController::ManipulationController(HINSTANCE h)
    : m_hInstance(h), m_overlay(nullptr), m_renderer(), m_running(false) {
}

ManipulationController::~ManipulationController() {
}

void ManipulationController::InitializeManipulation() {

   HINSTANCE hInstance = m_hInstance;
   // temporary values for x, y, width, height
   int x = 100;
   int y = 100;
   int w = 800;
   int h = 600;

   // initialize D3D11 device  
   Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;  
   Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;  
   D3D_FEATURE_LEVEL featureLevel;  


   HRESULT hr = D3D11CreateDevice(  
      nullptr, // use default adapter  
      D3D_DRIVER_TYPE_HARDWARE,  
      nullptr, // no software rasterizer  
      0, // No creation flags  
      nullptr, 0, // default feature levels  
      D3D11_SDK_VERSION,  
      &d3dDevice,  
      &featureLevel,  
      &d3dContext  
   );

   if (FAILED(hr)) {  
       throw std::runtime_error("Failed to create D3D11 device.");  
   }  


   // create the overlay window (using std::make_unique)
   m_overlay = std::make_unique<OverlayWindow>(hInstance, x, y, w, h);
   m_overlay->create();
   m_overlay->show();

   // initialize the duplicator
   m_duplicator.initialize(d3dDevice);
}

bool ManipulationController::run() {  
   // initialize D3D11 device & duplicator  
   Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;  
   Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;  
   D3D_FEATURE_LEVEL featureLevel;  

   HRESULT hr = D3D11CreateDevice(  
       nullptr, // use default adapter  
       D3D_DRIVER_TYPE_HARDWARE,  
       nullptr, // no software rasterizer  
       0, // no creation flags  
       nullptr, 0, // default feature levels  
       D3D11_SDK_VERSION,  
       &d3dDevice,  
       &featureLevel,  
       &d3dContext  
   );  

   if (FAILED(hr)) {  
       throw std::runtime_error("Failed to create D3D11 device.");  
   }  

   m_duplicator.initialize(d3dDevice);  

   // create overlay window  
   int x = 100, y = 100, w = 800, h = 600;  
   m_overlay = std::make_unique<OverlayWindow>(m_hInstance, x, y, w, h);  
   if (!m_overlay->create()) {  
       throw std::runtime_error("Failed to create overlay window.");  
   }  
   m_overlay->show();  

   // initialize VulkanRenderer with overlay HWND  
   HWND hwnd = m_overlay->handle();  
     m_renderer = std::make_unique<VulkanRenderer>(hwnd);  
     if (!m_renderer->initialize()) {  
         throw std::runtime_error("Failed to initialize VulkanRenderer.");  
     }

   // Start capture thread  
   m_running = true;  
   std::thread captureThread([this]() {  
       while (m_running) {  
           // acquire frames and pass shared handles  
          // m_duplicator.acquireFrame();  
       }  
   });  
   captureThread.detach();  

   // Enter main loop  
   mainLoop();  
   return true;  
}

void ManipulationController::mainLoop() {
	MSG msg;
	while (m_running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) m_running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// render one frame (renderer.render(scale))
	}
}
