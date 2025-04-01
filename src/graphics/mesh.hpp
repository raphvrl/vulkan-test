#pragma once

#include <vulkan/vulkan.h>
#include <glm/ext.hpp>

#include <vector>
#include <array>

#include "device.hpp"
#include "buffer.hpp"

namespace gfx
{

class Mesh
{

public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, uv);

            return attributeDescriptions;
        }
    };

    Mesh() = default;
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

    void init(
        Device &device,
        const std::vector<Vertex> &vertices,
        const std::vector<u32> &indices
    );

    void destroy();

    void bind(VkCommandBuffer cmd) const;
    void draw(VkCommandBuffer cmd) const;

public:
    void setTextureID(u32 textureID) { m_textureID = textureID; }
    u32 getTextureID() const { return m_textureID; }

private:
    Device *m_device = nullptr;

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    u32 m_vertexCount = 0;
    u32 m_indexCount = 0;

    u32 m_textureID = 0;
};

} // namespace gfx