#include "core/window/window.hpp"

#include "graphics/device.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/mesh.hpp"
#include "graphics/bindless_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/model_manager.hpp"

struct CamUBO
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct PushConstant
{
    alignas(16) glm::mat4 model;
    alignas(4) u32 index;
};

int main()
{
    core::Window window;
    window.init(1280, 720, "Vulkan");

    gfx::Device device;
    device.init(window, "Vulkan", {1, 0, 0});

    auto &bindlessManager = device.getBindlessManager();

    gfx::ModelManager modelManager;
    modelManager.init(device, bindlessManager);
    u32 cubeID = modelManager.loadModel("assets/models/bingus.gltf");

    gfx::Camera camera;
    camera.setPosition({0.0f, 0.0f, 10.0f});

    gfx::Buffer cameraBuffer;
    cameraBuffer.init(
        device,
        sizeof(CamUBO),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU
    );

    u32 cameraUboIndex = bindlessManager.addUBO(cameraBuffer);

    std::cout << "Camera UBO Index: " << cameraUboIndex << std::endl;

    auto binding = gfx::Mesh::Vertex::getBindingDescription();
    auto attributes = gfx::Mesh::Vertex::getAttributeDescriptions();

    gfx::Pipeline pipeline = gfx::Pipeline::Builder(device)
        .setShader("assets/shaders/mesh.vert.spv", VK_SHADER_STAGE_VERTEX_BIT)
        .setShader("assets/shaders/mesh.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
        .setColorFormat(device.getSwapchain().getFormat())
        .setVertexInput({
            .binding = &binding,
            .attribute = attributes.data(),
            .attributeCount = static_cast<u32>(attributes.size())
        })
        .addPushConstantRange({
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(PushConstant)
        })
        .setDepthTest(true)
        .setDepthWrite(true)
        .build();

    f32 deltaTime = 0.0f;
    f32 lastFrame = 0.0f;
    
    while (window.isOpen()) {
        window.pollEvents();

        f32 currentFrame = window.getTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        f32 speed = 10.0f * deltaTime;

        u32 width = window.getWidth();
        u32 height = window.getHeight();
        if (width == 0 || height == 0) {
            continue;
        }

        if (window.isKeyPressed(GLFW_KEY_W)) {
            glm::vec3 front = camera.getFront();
            camera.setPosition(camera.getPosition() + front * speed);
        }

        if (window.isKeyPressed(GLFW_KEY_S)) {
            glm::vec3 front = camera.getFront();
            camera.setPosition(camera.getPosition() - front * speed);
        }

        if (window.isKeyPressed(GLFW_KEY_A)) {
            glm::vec3 right = camera.getRight();
            camera.setPosition(camera.getPosition() - right * speed);
        }

        if (window.isKeyPressed(GLFW_KEY_D)) {
            glm::vec3 right = camera.getRight();
            camera.setPosition(camera.getPosition() + right * speed);
        }

        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            glm::vec3 up = camera.getUp();
            camera.setPosition(camera.getPosition() + up * speed);
        }

        if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            glm::vec3 up = camera.getUp();
            camera.setPosition(camera.getPosition() - up * speed);
        }

        glm::vec2 mouseRel = window.getMouseRel();

        if (window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            f32 sensitivity = 0.1f;
            mouseRel.x *= sensitivity;
            mouseRel.y *= sensitivity;

            f32 yaw = camera.getYaw();
            f32 pitch = camera.getPitch();

            yaw += mouseRel.x;
            pitch -= mouseRel.y;

            pitch = std::clamp(pitch, -89.0f, 89.0f);

            camera.setYaw(yaw);
            camera.setPitch(pitch);
        }

        camera.setAspect(window.getAspect());
        camera.update();

        CamUBO *cameraData = static_cast<CamUBO *>(cameraBuffer.map());
        cameraData->view = camera.getView();
        cameraData->proj = camera.getProjection();
        cameraBuffer.unmap();

        bindlessManager.update();

        VkCommandBuffer cmd = device.beginFrame();
        if (!cmd) {
            continue;
        }

        pipeline.bind(cmd);

        PushConstant pc = {
            .model = glm::mat4(1.0f),
            .index = cameraUboIndex
        };

        pipeline.push(
            cmd,
            VK_SHADER_STAGE_VERTEX_BIT,
            sizeof(PushConstant),
            &pc
        );

        modelManager.drawModel(cmd, cubeID);

        device.endFrame();
    }

    device.waitIdle();

    cameraBuffer.destroy();
    modelManager.destroy();
    pipeline.destroy();
    device.destroy();
    window.destroy();

    return 0;
}