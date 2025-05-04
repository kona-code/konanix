#include "VulkanRenderer.h"  
#include <windows.h>  
#include <stdexcept>  

VulkanRenderer::VulkanRenderer(HWND hwnd)
    : m_hwnd(hwnd),
	m_isRendering(false),
    m_instance(VK_NULL_HANDLE),
    m_device(VK_NULL_HANDLE),
    m_physical(VK_NULL_HANDLE),
    m_graphicsQueue(VK_NULL_HANDLE),
    m_presentQueue(VK_NULL_HANDLE),
    m_surface(VK_NULL_HANDLE),
    m_swapchain(VK_NULL_HANDLE),
    m_format(VK_FORMAT_B8G8R8A8_SRGB),
    m_extent({ 0, 0 }),
    m_pipeline(VK_NULL_HANDLE),
    m_pipelineLayout(VK_NULL_HANDLE),
    m_renderPass(VK_NULL_HANDLE),
    m_commandPool(VK_NULL_HANDLE),
    m_imageAvailable(VK_NULL_HANDLE),
    m_renderFinished(VK_NULL_HANDLE),
    m_externalImage(VK_NULL_HANDLE),
    m_externalMemory(VK_NULL_HANDLE),
    m_externalImageView(VK_NULL_HANDLE),
    m_renderFence(VK_NULL_HANDLE),
    m_zoomFactor(settings.getScreenZoom()) {
}

VulkanRenderer::~VulkanRenderer() { cleanup(); }

bool VulkanRenderer::initialize() {
    Logger::getInstance().logInfo(">-------------------[Logging Vulkan Initialization]-------------------<");

    // Create Vulkan instance
    Logger::getInstance().logInfo("Creating Vulkan instance...");
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "konanix";
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
        return false;
    }
    Logger::getInstance().logInfo("Vulkan instance created successfully.");

    // Create Vulkan surface
    Logger::getInstance().logInfo("Creating Vulkan surface...");
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = m_hwnd;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to create Vulkan surface.");
        return false;
    }
    Logger::getInstance().logInfo("Vulkan surface created successfully.");

    // Select physical device
    Logger::getInstance().logInfo("Selecting Vulkan physical device...");
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        Logger::getInstance().logError("Failed to find GPUs with Vulkan support.");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Check for required features and extensions
        if (true /* Add checks for required features */) {
            m_physical = device;
            break;
        }
    }

    if (m_physical == VK_NULL_HANDLE) {
        Logger::getInstance().logError("Failed to find a suitable GPU.");
        return false;
    }
    Logger::getInstance().logInfo("Vulkan physical device selected successfully.");

    // Create logical device
    Logger::getInstance().logInfo("Creating Vulkan logical device...");
    uint32_t queueFamilyIndex = 0; // Replace with actual queue family index

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
        return false;
    }

    vkGetDeviceQueue(m_device, queueFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, queueFamilyIndex, 0, &m_presentQueue);

    Logger::getInstance().logInfo("Vulkan logical device created successfully.");


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
		createFramebuffers();
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

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;

	// create fence for synchronization
	Logger::getInstance().logInfo("Creating Vulkan fence...");
    if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_renderFence) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to create render fence.");
        throw std::runtime_error("Failed to create render fence.");
    }
    Logger::getInstance().logInfo("Render fence created successfully.");

    Logger::getInstance().logInfo(">-------------------[Finished Vulkan Initilization]-------------------<");

    return true;
}


void VulkanRenderer::createFramebuffers() {

       Logger::getInstance().logInfo("Creating Vulkan swapchain image views...");  

       if (m_swapchainImages.empty()) {  
           Logger::getInstance().logError("m_swapchainImages is empty. Cannot create image views.");  
           throw std::runtime_error("m_swapchainImages is empty.");  
       }  

       m_swapchainImageViews.resize(m_swapchainImages.size());  

       for (size_t i = 0; i < m_swapchainImages.size(); ++i) {  
           VkImageViewCreateInfo viewInfo = {};  
           viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;  
           viewInfo.image = m_swapchainImages[i];  
           viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  
           viewInfo.format = m_format;  
           viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;  
           viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;  
           viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;  
           viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;  
           viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  
           viewInfo.subresourceRange.baseMipLevel = 0;  
           viewInfo.subresourceRange.levelCount = 1;  
           viewInfo.subresourceRange.baseArrayLayer = 0;  
           viewInfo.subresourceRange.layerCount = 1;  

           if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {  
               Logger::getInstance().logError("Failed to create image view for swapchain image " + std::to_string(i));  
               throw std::runtime_error("Failed to create image view.");  
           }  
       }  

       Logger::getInstance().logInfo("Vulkan swapchain image views created successfully.");  
    
    Logger::getInstance().logInfo("Creating Vulkan framebuffers...");

    if (m_swapchainImageViews.empty()) {
        Logger::getInstance().logError("m_swapchainImageViews is empty. Cannot create framebuffers.");
        throw std::runtime_error("m_swapchainImageViews is empty.");
    }

    m_framebuffers.resize(m_swapchainImages.size());

    for (size_t i = 0; i < m_swapchainImages.size(); ++i) {
        if (i >= m_swapchainImageViews.size() || !m_swapchainImageViews[i]) {
            Logger::getInstance().logError("Invalid or missing image view for swapchain image " + std::to_string(i));
            throw std::runtime_error("Invalid or missing image view for swapchain image.");
        }

        VkImageView attachments[] = {
			m_swapchainImageViews[i]

        };
        if (m_swapchainImageViews[i] == VK_NULL_HANDLE) {
            Logger::getInstance().logError("Swapchain image view at index " + std::to_string(i) + " is null.");
            throw std::runtime_error("Swapchain image view is null.");
        }

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            Logger::getInstance().logError("Failed to create framebuffer for swapchain image " + std::to_string(i));
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }

    Logger::getInstance().logInfo("Vulkan framebuffers created successfully.");
}


bool VulkanRenderer::importExternalImage(HANDLE sharedHandle, int width, int height) {
    VkExternalMemoryImageCreateInfo externalMemoryInfo = {};
    externalMemoryInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
    externalMemoryInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.pNext = &externalMemoryInfo;

    if (vkCreateImage(m_device, &imageCreateInfo, nullptr, &m_externalImage) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to create external image.");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_externalImage, &memRequirements);

    VkImportMemoryWin32HandleInfoKHR importInfo = {};
    importInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
    importInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
    importInfo.handle = sharedHandle;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = 0; // Find suitable memory type
    allocInfo.pNext = &importInfo;

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_externalMemory) != VK_SUCCESS) {
        Logger::getInstance().logError("Failed to allocate memory for external image.");
        return false;
    }

    vkBindImageMemory(m_device, m_externalImage, m_externalMemory, 0);

    return true;
}


void VulkanRenderer::render(float scale) {
    // Wait for the previous frame to finish
    vkWaitForFences(m_device, 1, &m_renderFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_renderFence);

    // Acquire the next image from the swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailable, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Handle swapchain recreation if necessary
        createFramebuffers();
        return;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swapchain image!");
    }

    // Record command buffer for the current frame
    VkCommandBuffer commandBuffer = m_commandBuffers[imageIndex];
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = m_renderPassInfo[imageIndex];
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Add rendering commands here (e.g., vkCmdBindPipeline, vkCmdDraw, etc.)
    vkCmdEndRenderPass(commandBuffer);

    vkEndCommandBuffer(commandBuffer);

    // Submit the command buffer to the graphics queue
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    VkSemaphore signalSemaphores[] = { m_renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_renderFence);

    // Present the image to the swapchain
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Handle swapchain recreation if necessary
        createFramebuffers();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image!");
    }
}

void VulkanRenderer::InitializeKonanixStart(float scale) {
    // render the bottom image (simulates desktop wallpaper)
    render(scale);

    // Render the top image (simulates desktop wallpaper)
    render(scale * settings.getScreenZoom());
}

void VulkanRenderer::RenderKonanixStart(const std::vector<WindowInfo>& windows) {
    // Ensure Vulkan is initialized
    if (!m_isRendering) {
        Logger::getInstance().logError("VulkanRenderer is not initialized.");
        return;
    }

    // Iterate through each window and render its thumbnail
    for (const auto& window : windows) {
        try {
            // Capture the window's content as a texture
            HDC hdcWindow = GetDC(window.hwnd);
            HDC hdcMem = CreateCompatibleDC(hdcWindow);

            RECT rect = window.rect;
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
            SelectObject(hdcMem, hBitmap);

            // Copy the window's content into the bitmap
            BitBlt(hdcMem, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

            // Create a Vulkan texture from the bitmap
            HANDLE sharedHandle = nullptr;
            VkImage vulkanImage = importBitmapToVulkan(hBitmap, width, height, sharedHandle);

            // Release GDI resources
            DeleteObject(hBitmap);
            DeleteDC(hdcMem);
            ReleaseDC(window.hwnd, hdcWindow);

            // Render the Vulkan texture as a thumbnail
            renderThumbnail(vulkanImage, rect, window.title);

            // Clean up Vulkan resources for the thumbnail
            cleanupVulkanImage(vulkanImage, sharedHandle);
        }
        catch (const std::exception& ex) {
            Logger::getInstance().logError("Failed to render thumbnail for window: " + std::string(ex.what()));
        }
    }
}

std::vector<WindowInfo> enumerateOpenWindows() {
    std::vector<WindowInfo> windows;

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        std::vector<WindowInfo>* windows = reinterpret_cast<std::vector<WindowInfo>*>(lParam);

        if (IsWindowVisible(hwnd)) {
            wchar_t title[256];
            GetWindowTextW(hwnd, title, sizeof(title) / sizeof(wchar_t));

            RECT rect;
            GetWindowRect(hwnd, &rect);

            if (wcslen(title) > 0) {
                windows->push_back({ hwnd, title, rect });
            }
        }
        return TRUE;
        }, reinterpret_cast<LPARAM>(&windows));

    return windows;
}

void VulkanRenderer::cleanup() {
    for (auto framebuffer : m_framebuffers) {
        if (framebuffer) vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    m_framebuffers.clear();

    for (auto imageView : m_swapchainImageViews) {
        if (imageView) vkDestroyImageView(m_device, imageView, nullptr);
    }
    m_swapchainImageViews.clear();

    if (m_renderFence) vkDestroyFence(m_device, m_renderFence, nullptr);
    if (m_imageAvailable) vkDestroySemaphore(m_device, m_imageAvailable, nullptr);
    if (m_renderFinished) vkDestroySemaphore(m_device, m_renderFinished, nullptr);
    if (m_commandPool) vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    if (m_swapchain) vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    if (m_surface) vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    if (m_device) vkDestroyDevice(m_device, nullptr);
    if (m_instance) vkDestroyInstance(m_instance, nullptr);
}

