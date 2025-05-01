#include "Controller.h"
#include <memory>

void ManipulationController::InitializeManipulation() {
   // define missing variables
   HINSTANCE hInstance = m_hInstance; // use member variable (m_hInstance)
   // temporary values for x, y, width, height
   int x = 100;
   int y = 100;
   int w = 800;
   int h = 600;

   ID3D11Device* d3dDevice = nullptr; // todo: replace with actual initialization

   // create the overlay window (using std::make_unique)
   m_overlay = std::make_unique<OverlayWindow>(hInstance, x, y, w, h);
   m_overlay->create();
   m_overlay->show();

   // initialize the duplicator
   m_duplicator.initialize(d3dDevice);
}
ManipulationController::ManipulationController(HINSTANCE h)
	: m_hInstance(h), m_overlay(nullptr), m_renderer(nullptr), m_running(false) {
}

ManipulationController::~ManipulationController() {
}

bool ManipulationController::run() {
	// todo:
	// 1) Initialize D3D11 device & duplicator
	// 2) Create overlay window
	// 3) Initialize VulkanRenderer with overlay HWND
	// 4) Start capture thread that acquires frames and passes shared handles
	m_running = true;
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
