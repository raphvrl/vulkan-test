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
        

        Pipeline build();

    private:
        Device &m_device;

        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

        VertexInput m_vertexInput = {};
        bool m_vertexInputSet = false;

        VkFormat m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;

        std::vector<char> readFile(const std::string &filename);
        VkShaderModule createShaderModule(const std::vector<char> &code);

    };

    Pipeline() = default;
    void destroy();

    void bind(VkCommandBuffer cmd);

private:
    friend class Builder;

    Device *m_device;
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;

};

} // namespace gfx