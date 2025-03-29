#include "core/window/window.hpp"

#include "graphics/device.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/mesh.hpp"

int main()
{
    core::Window window;

    gfx::Device device;
    gfx::Pipeline pipeline;
    gfx::Mesh mesh;

    window.init(800, 600, "Vulkan");
    device.init(window, "Vulkan", {1, 0, 0});

    auto binding = gfx::Mesh::Vertex::getBindingDescription();
    auto attributes = gfx::Mesh::Vertex::getAttributeDescriptions();

    pipeline = gfx::Pipeline::Builder(device)
        .setShader(
            "assets/shaders/triangle.vert.spv",
            VK_SHADER_STAGE_VERTEX_BIT
        )
        .setShader(
            "assets/shaders/triangle.frag.spv",
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
        .build();

    std::vector<gfx::Mesh::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},    
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   
        
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, 
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},   
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}   
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 7, 6, 6, 5, 4,
        0, 3, 7, 7, 4, 0,
        1, 5, 6, 6, 2, 1,
        3, 2, 6, 6, 7, 3,
        0, 4, 5, 5, 1, 0
    };
    
    mesh.init(device, vertices, indices);

    while (window.isOpen()) {
        window.pollEvents();

        u32 width = window.getWidth();
        u32 height = window.getHeight();

        if (width == 0 || height == 0) {
            continue;
        }

        auto cmd = device.beginFrame();
        if (cmd) {
            pipeline.bind(cmd);

            mesh.bind(cmd);
            mesh.draw(cmd);

            device.endFrame();
        }
    }

    device.waitIdle();

    mesh.destroy();
    pipeline.destroy();
    device.destroy();

    window.destroy();

    return 0;
}