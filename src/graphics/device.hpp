#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "core/types.hpp"
#include "core/window/window.hpp"

#include "utils/init.hpp"
#include "swapchain.hpp"

namespace gfx
{

class Device
{

public:
    Device() = default;
    ~Device() = default;

    void init(
        core::Window &window,
        const std::string &appName,
        const Version &version
    );

    void destroy();

    VkCommandBuffer beginFrame();
    void endFrame();

    void waitIdle();

public:
    VkInstance getInstance() const { return m_instance; }
    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    VkDevice getDevice() const { return m_device; }

    VkSurfaceKHR getSurface() const { return m_surface; }
    Swapchain &getSwapchain() { return m_swapchain; }

    VmaAllocator getAllocator() const { return m_allocator; }

    VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue getPresentQueue() const { return m_presentQueue; }

private:
    struct FrameData
    {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    };

private:
    core::Window *m_window = nullptr;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    Swapchain m_swapchain;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    vk::QueueFamilyIndices m_queueFamilyIndices;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> m_frames;
    u32 m_currentFrame = 0;
    u32 m_imageIndex = 0;

private:
    void recreateSwapchain();

};

} // namespace gfx