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
    Logger::getInstance().logInfo(">-------------------[Logging Vulkan Initilization]-------------------<");

    Logger::getInstance().logInfo("Initializing VulkanRenderer...");

    // create Vulkan instance
    Logger::getInstance().logInfo("Creating Vulkan instance...");
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Konanix";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to create Vulkan instance.");
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    Logger::getInstance().logInfo("Vulkan instance created successfully.");


    // create surface
    Logger::getInstance().logInfo("Creating Vulkan surface...");
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = m_hwnd;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to create Vulkan surface.");
        throw std::runtime_error("Failed to create Vulkan surface.");
    }
    Logger::getInstance().logInfo("Vulkan surface created successfully.");

    // select physical device
	Logger::getInstance().logInfo("Selecting Vulkan physical device...");
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
		Logger::getInstance().logError("Failed to find GPUs with Vulkan support.");
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // check for required features
        if (true /* add checks here */) {
            m_physical = device;
            break;
        }
    }

    if (m_physical == VK_NULL_HANDLE) {
		Logger::getInstance().logError("Failed to find a suitable GPU.");
        throw std::runtime_error("Failed to find a suitable GPU.");
    }
    Logger::getInstance().logInfo("Vulkan physical device created successfully.");

    // create logical device
	Logger::getInstance().logInfo("Creating Vulkan logical device...");
    uint32_t queueFamilyIndex = 0;

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;

    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(m_physical, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
		Logger::getInstance().logError("Failed to create logical device.");
        throw std::runtime_error("Failed to create logical device.");
    }

    vkGetDeviceQueue(m_device, queueFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, queueFamilyIndex, 0, &m_presentQueue);

    // set swapchain extent
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_extent = { static_cast<uint32_t>(rect.right - rect.left), static_cast<uint32_t>(rect.bottom - rect.top) };

    // create swapchain
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_surface;
    swapchainCreateInfo.minImageCount = 2; // Double buffering
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageExtent = m_extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        Logger::getInstance().logInfo("Failed to create swapchain.");
        throw std::runtime_error("Failed to create swapchain.");
    }
    Logger::getInstance().logInfo("Vulkan swapchain created successfully.");

	// get swapchain images
	Logger::getInstance().logInfo("Getting swapchain images...");
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());


    // set up command buffers
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		Logger::getInstance().logError("Failed to create command pool.");
        throw std::runtime_error("Failed to create command pool.");
    }
    Logger::getInstance().logInfo("Vulkan command pool created successfully.");

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_swapchainImages.size());

    m_commandBuffers.resize(m_swapchainImages.size());
    if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		Logger::getInstance().logError("Failed to allocate command buffers.");
        throw std::runtime_error("Failed to allocate command buffers.");
    }
    Logger::getInstance().logInfo("Successfully allocated Vulkan command buffers.");

    m_renderPassInfo.resize(m_swapchainImages.size());
    for (size_t i = 0; i < m_swapchainImages.size(); ++i) {
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffers[i]; // ensures framebuffers are created
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_extent;

        VkClearValue clearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        m_renderPassInfo[i] = renderPassInfo;
    }

    // create semaphores
	Logger::getInstance().logInfo("Creating Vulkan semaphores...");
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailable) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinished) != VK_SUCCESS) {
		Logger::getInstance().logError("Failed to create semaphores.");
        throw std::runtime_error("Failed to create semaphores.");
    }
    Logger::getInstance().logInfo("Vulkan semaphores created successfully.");

    Logger::getInstance().logInfo(">-------------------[Finished Vulkan Initilization]-------------------<");

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
    Logger::getInstance().logInfo("Starting render with scale: " + std::to_string(scale));



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
   // 
    // acquire swapchain image
   uint32_t imgIndex;
   VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailable, VK_NULL_HANDLE, &imgIndex);
   if (result != VK_SUCCESS) {
       Logger::getInstance().logError("Failed to acquire swapchain image. Error code: " + std::to_string(result));
       throw std::runtime_error("Failed to acquire swapchain image.");
   }
   Logger::getInstance().logInfo("Acquired swapchain image index: " + std::to_string(imgIndex));
   //uint32_t imgIndex;  
   //vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,  
   //    m_imageAvailable, VK_NULL_HANDLE, &imgIndex);  

   auto& cmd = m_commandBuffers[imgIndex];  
   vkResetCommandBuffer(cmd, 0);  
   VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };  
   vkBeginCommandBuffer(cmd, &beginInfo);  

   // begin render pass  
   if (m_renderPassInfo.empty()) {  
	   Logger::getInstance().logError("m_renderPassInfo is undefined or empty.");
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