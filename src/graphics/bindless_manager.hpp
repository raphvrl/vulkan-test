#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

#include "core/types.hpp"
#include "device.hpp"
#include "buffer.hpp"

namespace gfx
{

class BindlessManager
{

public:
    enum class ResourceType
    {
        UBO,
        SSBO
    };

    BindlessManager() = default;
    ~BindlessManager() = default;

    void init(Device &device);
    void destroy();

    u32 addUBO(
        const Buffer &buffer,
        VkDeviceSize offset = 0,
        VkDeviceSize range = 0
    );

    u32 addSSBO(
        const Buffer &buffer,
        VkDeviceSize offset = 0,
        VkDeviceSize range = 0
    );

    void removeResource(u32 id);

    void update();

public:
    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
    VkDescriptorSet getDescriptorSet() const { return m_descriptorSet; }

private:
    Device *m_device = nullptr;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    struct ResourceSlot
    {
        ResourceType type;
        u32 binding;
        u32 arrayIndex;

        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceSize offset = 0;
        VkDeviceSize range = 0;

        bool isDirty = false;
        bool isUsed = false;
    };

    std::vector<ResourceSlot> m_resources;
    std::vector<u32> m_dirtyResources;
    std::mutex m_mutex;

    u32 m_nextUboIndex = 0;
    u32 m_nextSsboIndex = 0;

    static constexpr u32 MAX_UBOS = 256;
    static constexpr u32 MAX_SSBOS = 256;

    static constexpr u32 UBO_BINDING = 0;
    static constexpr u32 SSBO_BINDING = 1;

    u32 addResourceInternal(
        ResourceType type,
        u32 binding,
        u32 &nextIndex,
        u32 maxCount
    );

};

} // namespace gfx
