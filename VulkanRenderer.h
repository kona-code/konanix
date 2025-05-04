#pragma once  
#include <windows.h>  
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "settings.h"
#include "Logger.h"
#include <string>

struct WindowInfo {
    HWND hwnd;
    std::wstring title;
    RECT rect;
};

std::vector<WindowInfo> enumerateOpenWindows();

class VulkanRenderer {  
public:  
   VulkanRenderer(HWND hwnd);
   ~VulkanRenderer();  
   bool initialize();  
   bool importExternalImage(HANDLE sharedHandle, int width, int height);  
   void render(float scale);  
   void cleanup();  
   void StopRendering() { m_isRendering = false; }
   void createFramebuffers();
   void InitializeKonanixStart(float scale);
   void RenderKonanixStart(const std::vector<WindowInfo>& windows);


private:  
   Settings settings;

   HWND m_hwnd;  
   VkInstance m_instance;  
   VkDevice m_device;  
   VkPhysicalDevice m_physical;  
   VkQueue m_graphicsQueue, m_presentQueue;  
   VkSurfaceKHR m_surface;  
   VkSwapchainKHR m_swapchain;  
   std::vector<VkImage> m_swapchainImages;  
   std::vector<VkImageView> m_swapchainImageViews;  
   VkFormat m_format = VK_FORMAT_B8G8R8A8_SRGB;  
   VkExtent2D m_extent;  
   VkPipeline m_pipeline;  
   VkPipelineLayout m_pipelineLayout;  
   VkRenderPass m_renderPass;  
   VkCommandPool m_commandPool;  
   std::vector<VkCommandBuffer> m_commandBuffers;  
   VkSemaphore m_imageAvailable;  
   VkSemaphore m_renderFinished;  
   std::vector<VkRenderPassBeginInfo> m_renderPassInfo;  
   std::vector<VkFramebuffer> m_framebuffers;
   VkFence m_renderFence;

   // external image resources  
   VkImage m_externalImage;  
   VkDeviceMemory m_externalMemory;  
   VkImageView m_externalImageView;  
   
   double m_zoomFactor;
   const int targetFPS = settings.getFrameLimit(); // target frame rate
   bool m_isRendering;
};

