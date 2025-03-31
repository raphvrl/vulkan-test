#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <fstream>

#include "device.hpp"
#include "utils/utils.hpp"

namespace gfx
{

struct VertexInput
{
    VkVertexInputBindingDescription *binding = {};
    VkVertexInputAttributeDescription *attribute = {};
    u32 attributeCount = 0;
};

class Pipeline
{

public:
    class Builder
    {

    public:
        Builder(Device &device);
        ~Builder() = default;

        Builder &setShader(const std::string &path, VkShaderStageFlagBits stage);
        Builder &setVertexInput(const VertexInput &vertexInput);
        Builder &setColorFormat(VkFormat format);
        Builder &setDescriptorSetLayout(VkDescriptorSetLayout layout);
        Builder &addPushConstantRange(VkPushConstantRange range);
        Builder &setDepthTest(bool enable);
        Builder &setDepthWrite(bool enable);
        

        Pipeline build();

    private:
        Device &m_device;

        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

        VertexInput m_vertexInput = {};
        bool m_vertexInputSet = false;

        VkFormat m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;

        VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
        std::vector<VkPushConstantRange> m_pushConstantRanges;

        bool m_depthTest = false;
        bool m_depthWrite = false;

        std::vector<char> readFile(const std::string &filename);
        VkShaderModule createShaderModule(const std::vector<char> &code);

    };

    Pipeline() = default;
    void destroy();

    void bind(VkCommandBuffer cmd);

    void push(
        VkCommandBuffer cmd,
        VkShaderStageFlagBits stage,
        VkDeviceSize size,
        void *data
    );

    void bindDescriptorSet(
        VkCommandBuffer cmd,
        VkDescriptorSet descriptorSet
    );

private:
    friend class Builder;

    Device *m_device;
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;

};

} // namespace gfx