#include "core/window/window.hpp"

#include "graphics/device.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/model.hpp"
#include "graphics/bindless_manager.hpp"
#include "graphics/camera.hpp"

struct CamUBO
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

int main()
{
    core::Window window;

    gfx::Device device;
    gfx::Pipeline pipeline;
    gfx::Model model;
    gfx::Camera camera;

    gfx::BindlessManager bindlessManager;

    window.init(800, 600, "Vulkan");
    device.init(window, "Vulkan", {1, 0, 0});

    bindlessManager.init(device);

    gfx::Buffer cameraBuffer;
    cameraBuffer.init(
        device,
        sizeof(CamUBO),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU
    );

    u32 colorUboIndex = bindlessManager.addUBO(cameraBuffer);

    auto binding = gfx::Mesh::Vertex::getBindingDescription();
    auto attributes = gfx::Mesh::Vertex::getAttributeDescriptions();

    pipeline = gfx::Pipeline::Builder(device)
        .setShader(
            "assets/shaders/mesh.vert.spv",
            VK_SHADER_STAGE_VERTEX_BIT
        )
        .setShader(
            "assets/shaders/mesh.frag.spv",
            VK_SHADER_STAGE_FRAGMENT_BIT
        )
        .setColorFormat(
            device.getSwapchain().getFormat()
        )
        .setVertexInput(
            {
                .binding = &binding,
                .attribute = attributes.data(),
                .attributeCount = static_cast<u32>(attributes.size())
            }
        )
        .setDescriptorSetLayout(bindlessManager.getDescriptorSetLayout())
        .addPushConstantRange(
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(u32)
            }
        )
        .build();

    model.load(device, bindlessManager, "assets/models/cube.gltf");

    f32 time = 0.0f;
    f32 lastTime = 0.0f;

    while (window.isOpen()) {
        window.pollEvents();

        u32 width = window.getWidth();
        u32 height = window.getHeight();

        if (width == 0 || height == 0) {
            continue;
        }

        camera.setAspect(static_cast<f32>(width) / static_cast<f32>(height));

        camera.update();

        f32 currentTime = static_cast<f32>(glfwGetTime());
        f32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        time += deltaTime;

        UNUSED(time);

        CamUBO *cameraData = static_cast<CamUBO *>(cameraBuffer.map());
        cameraData->view = camera.getView();
        cameraData->proj = camera.getProjection();
        cameraBuffer.unmap();

        bindlessManager.update();

        auto cmd = device.beginFrame();
        if (cmd) {
            pipeline.bind(cmd);
            
            VkDescriptorSet descriptorSet = bindlessManager.getDescriptorSet();
            pipeline.bindDescriptorSet(
                cmd,
                descriptorSet
            );

            pipeline.push(
                cmd,
                VK_SHADER_STAGE_VERTEX_BIT,
                sizeof(u32),
                &colorUboIndex
            );

            model.draw(cmd);

            device.endFrame();
        }
    }

    device.waitIdle();

    cameraBuffer.destroy();
    model.destroy();
    bindlessManager.destroy();
    pipeline.destroy();
    device.destroy();

    window.destroy();

    return 0;
}