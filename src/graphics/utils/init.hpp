#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vk_mem_alloc.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <stdexcept>
#include <algorithm>

#include "core/types.hpp"

#include "utils.hpp"

namespace gfx
{

struct Version
{
    u32 major = 0;
    u32 minor = 0;
    u32 patch = 0;
};

namespace vk
{

VkInstance createInstance(
    const std::string &appName,
    const Version &version
);

VkSurfaceKHR createSurface(
    GLFWwindow *window,
    VkInstance instance
);

VkPhysicalDevice pickPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface
);

VkDevice createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface
);

struct QueueFamilyIndices
{
    std::optional<u32> graphicsFamily;
    std::optional<u32> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device,
    VkSurfaceKHR surface
);

VkQueue getGraphicsQueue(
    VkDevice device,
    QueueFamilyIndices indices,
    u32 queueIndex = 0
);

VkQueue getPresentQueue(
    VkDevice device,
    QueueFamilyIndices indices,
    u32 queueIndex = 0
);

VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);

void destroyDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger
);

VkSwapchainKHR createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    u32 width,
    u32 height,
    VkFormat preferredFormat = VK_FORMAT_B8G8R8A8_UNORM,
    VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR,
    u32 imageCount = 0,
    VkFormat* selectedFormat = nullptr,
    VkExtent2D* selectedExtent = nullptr,
    VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE
);

std::vector<VkImage> getSwapchainImages(
    VkDevice device,
    VkSwapchainKHR swapchain
);

std::vector<VkImageView> createSwapchainImageViews(
    VkDevice device,
    const std::vector<VkImage>& swapchainImages,
    VkFormat swapchainImageFormat
);

VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device, VkFenceCreateFlags flags = 0);

VkCommandPool createCommandPool(
    VkDevice device,
    u32 queueFamilyIndex,
    VkCommandPoolCreateFlags flags = 0
);

VkCommandBuffer createCommandBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
);

VmaAllocator createAllocator(
    VkInstance instance,
    VkDevice device,
    VkPhysicalDevice physicalDevice
);

} // namespace vk

} // namespace gfx