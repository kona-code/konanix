#include "VulkanRenderer.h"  
#include <windows.h>  
#include <stdexcept>  

VulkanRenderer::VulkanRenderer(HWND hwnd)
    : m_hwnd(hwnd),
    m_instance(VK_NULL_HANDLE),
    m_device(VK_NULL_HANDLE),
    m_physical(VK_NULL_HANDLE),
    m_graphicsQueue(VK_NULL_HANDLE),
    m_presentQueue(VK_NULL_HANDLE),
    m_surface(VK_NULL_HANDLE),
    m_swapchain(VK_NULL_HANDLE),
    m_format(VK_FORMAT_UNDEFINED),
    m_extent({ 0, 0 }),
    m_pipeline(VK_NULL_HANDLE),
    m_pipelineLayout(VK_NULL_HANDLE),
    m_renderPass(VK_NULL_HANDLE),
    m_commandPool(VK_NULL_HANDLE),
    m_imageAvailable(VK_NULL_HANDLE),
    m_renderFinished(VK_NULL_HANDLE),
    m_externalImage(VK_NULL_HANDLE),
    m_externalMemory(VK_NULL_HANDLE),
    m_externalImageView(VK_NULL_HANDLE) {
}

VulkanRenderer::~VulkanRenderer() { cleanup(); }

bool VulkanRenderer::initialize() {
    // create Vulkan instance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Konanix";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }


    return true;
}
bool VulkanRenderer::importExternalImage(HANDLE sharedHandle, int width, int height) {
    // todo:
    // (1) VkImageCreateInfo + VkExternalMemoryImageCreateInfo  
    // (2) Query mem requirements & import via VkImportMemoryWin32HandleInfoKHR  
    // (3) vkBindImageMemory + create image view  
    return true;
}

void VulkanRenderer::render(float scale) {  
   if (!this) {  
       MessageBox(NULL, L"VulkanRenderer instance is null.\nKonanix will now close for stability reasons.", L"Konanix Runtime Error", MB_ICONERROR);
       exit(1);
   }  

   // compute translation in NDC  
   float cx = m_extent.width / 2.0f;  
   float cy = m_extent.height / 2.0f;  
   float tx = ((1.0f - scale) * cx) / cx;  
   float ty = -((1.0f - scale) * cy) / cy;  

   // acquire swapchain image  
   uint32_t imgIndex;  
   vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,  
       m_imageAvailable, VK_NULL_HANDLE, &imgIndex);  

   auto& cmd = m_commandBuffers[imgIndex];  
   vkResetCommandBuffer(cmd, 0);  
   VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };  
   vkBeginCommandBuffer(cmd, &beginInfo);  

   // begin render pass  
   if (m_renderPassInfo.empty()) {  
       throw std::runtime_error("m_renderPassInfo is undefined or empty.");  
   }  
   vkCmdBeginRenderPass(cmd, &m_renderPassInfo[imgIndex], VK_SUBPASS_CONTENTS_INLINE);  

   // bind pipeline and descriptors  
   vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);  

   // push constants (scale + translate)  
   struct Push { float scale, tx, ty; } pc{ scale, tx, ty };  
   vkCmdPushConstants(cmd, m_pipelineLayout,  
       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);  

   // draw fullscreen quad (4 vertices)  
   vkCmdDraw(cmd, 4, 1, 0, 0);  

   // end render pass and command buffer  
   vkCmdEndRenderPass(cmd);  
   vkEndCommandBuffer(cmd);  

   // submit and present  
   VkSubmitInfo si{ VK_STRUCTURE_TYPE_SUBMIT_INFO };  
   si.waitSemaphoreCount = 1; si.pWaitSemaphores = &m_imageAvailable;  
   VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  
   si.pWaitDstStageMask = &waitMask;  
   si.commandBufferCount = 1; si.pCommandBuffers = &cmd;  
   si.signalSemaphoreCount = 1; si.pSignalSemaphores = &m_renderFinished;  
   vkQueueSubmit(m_graphicsQueue, 1, &si, VK_NULL_HANDLE);  

   VkPresentInfoKHR pi{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };  
   pi.waitSemaphoreCount = 1; pi.pWaitSemaphores = &m_renderFinished;  
   pi.swapchainCount = 1; pi.pSwapchains = &m_swapchain;  
   pi.pImageIndices = &imgIndex;  
   vkQueuePresentKHR(m_presentQueue, &pi);  
}

void VulkanRenderer::cleanup() {
    // vkDeviceWaitIdle + destroy all Vk objects  
}