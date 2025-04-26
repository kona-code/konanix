#pragma once  
#include <windows.h>  
#include <vulkan/vulkan.h>  
#include <vector>  

class VulkanRenderer {  
public:  
   VulkanRenderer(HWND hwnd);  
   ~VulkanRenderer();  
   bool initialize();  
   bool importExternalImage(HANDLE sharedHandle, int width, int height);  
   void render(float scale);  
   void cleanup();  

private:  
   HWND m_hwnd;  
   VkInstance m_instance;  
   VkDevice m_device;  
   VkPhysicalDevice m_physical;  
   VkQueue m_graphicsQueue, m_presentQueue;  
   VkSurfaceKHR m_surface;  
   VkSwapchainKHR m_swapchain;  
   std::vector<VkImage> m_swapchainImages;  
   std::vector<VkImageView> m_swapchainImageViews;  
   VkFormat m_format;  
   VkExtent2D m_extent;  
   VkPipeline m_pipeline;  
   VkPipelineLayout m_pipelineLayout;  
   VkRenderPass m_renderPass;  
   VkCommandPool m_commandPool;  
   std::vector<VkCommandBuffer> m_commandBuffers;  
   VkSemaphore m_imageAvailable;  
   VkSemaphore m_renderFinished;  
   std::vector<VkRenderPassBeginInfo> m_renderPassInfo;  

   // external image resources  
   VkImage m_externalImage;  
   VkDeviceMemory m_externalMemory;  
   VkImageView m_externalImageView;  
};