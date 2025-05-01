#pragma once  

#include "DesktopDuplicator.h"  
#include "VulkanRenderer.h"  
#include "OverlayWindow.h"  
#include <Windows.h>  
#include <memory>  

class ManipulationController {  
public:  
   ManipulationController(HINSTANCE hInstance);  
   ~ManipulationController();  
   bool run();  
   void InitializeManipulation();  
   std::unique_ptr <VulkanRenderer> m_renderer;
   std::unique_ptr<OverlayWindow> m_overlay;
private:
   HINSTANCE m_hInstance;
   DesktopDuplicator m_duplicator;
   bool m_running;  
   void mainLoop();  
};