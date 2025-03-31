#include "pipeline.hpp"

namespace gfx
{

Pipeline::Builder::Builder(Device &device) : m_device(device)
{
}

Pipeline::Builder &Pipeline::Builder::setShader(
    const std::string &path,
    VkShaderStageFlagBits stage
)
{
    auto code = readFile(path);
    auto shaderModule = createShaderModule(code);

    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = "main";

    m_shaderStages.push_back(shaderStageInfo);

    return *this;
}

Pipeline::Builder &Pipeline::Builder::setVertexInput(
    const VertexInput &vertexInput
)
{
    m_vertexInput = vertexInput;
    m_vertexInputSet = true;
    return *this;
}

Pipeline::Builder &Pipeline::Builder::setColorFormat(VkFormat format)
{
    m_colorFormat = format;
    return *this;
}

Pipeline::Builder &Pipeline::Builder::setDescriptorSetLayout(
    VkDescriptorSetLayout layout
)
{
    m_descriptorSetLayout = layout;
    return *this;
}

Pipeline::Builder &Pipeline::Builder::addPushConstantRange(
    VkPushConstantRange range
)
{
    m_pushConstantRanges.push_back(range);
    return *this;
}

Pipeline::Builder &Pipeline::Builder::setDepthTest(bool enable)
{
    m_depthTest = enable;
    return *this;
}

Pipeline::Builder &Pipeline::Builder::setDepthWrite(bool enable)
{
    m_depthWrite = enable;
    return *this;
}

Pipeline Pipeline::Builder::build()
{
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = m_vertexInputSet ? 1 : 0;
    vertexInputInfo.pVertexBindingDescriptions = m_vertexInput.binding;
    vertexInputInfo.vertexAttributeDescriptionCount = m_vertexInput.attributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = m_vertexInput.attribute;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRenderingCreateInfoKHR renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &m_colorFormat;
    renderingInfo.depthAttachmentFormat = m_device.getDepthFormat();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = m_depthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = m_depthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<u32>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = m_descriptorSetLayout ? 1 : 0;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<u32>(
        m_pushConstantRanges.size()
    );
    pipelineLayoutInfo.pPushConstantRanges = m_pushConstantRanges.data();

    VkResult res = vkCreatePipelineLayout(
        m_device.getDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout
    );

    vk::check(res, "failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingInfo;
    pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    res = vkCreateGraphicsPipelines(
        m_device.getDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &pipeline
    );

    vk::check(res, "failed to create graphics pipeline!");

    for (auto& shaderStage : m_shaderStages) {
        vkDestroyShaderModule(m_device.getDevice(), shaderStage.module, nullptr);
    }

    Pipeline pipelineObj;

    pipelineObj.m_device = &m_device;
    pipelineObj.m_pipeline = pipeline;
    pipelineObj.m_pipelineLayout = pipelineLayout;

    return pipelineObj;
}

std::vector<char> Pipeline::Builder::readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filename);
    }

    usize fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
} 

VkShaderModule Pipeline::Builder::createShaderModule(
    const std::vector<char> &code
)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    VkResult res = vkCreateShaderModule(
        m_device.getDevice(),
        &createInfo,
        nullptr,
        &shaderModule
    );

    vk::check(res, "failed to create shader module!");

    return shaderModule;
}

void Pipeline::destroy()
{
    vkDestroyPipeline(m_device->getDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device->getDevice(), m_pipelineLayout, nullptr);
}

void Pipeline::bind(VkCommandBuffer cmd)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

void Pipeline::push(
    VkCommandBuffer cmd,
    VkShaderStageFlagBits stage,
    VkDeviceSize size,
    void *data
)
{
    vkCmdPushConstants(
        cmd,
        m_pipelineLayout,
        stage,
        0,
        size,
        data
    );
}

void Pipeline::bindDescriptorSet(
    VkCommandBuffer cmd,
    VkDescriptorSet descriptorSet
)
{
    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout,
        0,
        1,
        &descriptorSet,
        0,
        nullptr
    );
}

} // namespace gfx