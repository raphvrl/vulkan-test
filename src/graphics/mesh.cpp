#include "mesh.hpp"

namespace gfx
{

void Mesh::init(
    Device& device,
    const std::vector<Vertex>& vertices,
    const std::vector<u32>& indices)
{
    m_device = &device;
    m_vertexCount = static_cast<u32>(vertices.size());
    m_indexCount = static_cast<u32>(indices.size());

    VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();
VkDeviceSize indexBufferSize = sizeof(u32) * indices.size();

    m_vertexBuffer.init(
        device,
        vertexBufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU
    );

    m_vertexBuffer.uploadData(
        vertices.data(),
        m_vertexCount
    );

    if (m_indexCount > 0) {
        m_indexBuffer.init(
            device,
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU
        );

        m_indexBuffer.uploadData(
            indices.data(),
            m_indexCount
        );
    }
}

void Mesh::destroy()
{
    m_vertexBuffer.destroy();
    m_indexBuffer.destroy();
}

void Mesh::bind(VkCommandBuffer cmd) const
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer.getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

    if (m_indexCount > 0)
    {
        vkCmdBindIndexBuffer(
            cmd,
            m_indexBuffer.getBuffer(),
            0,
            VK_INDEX_TYPE_UINT32
        );
    }
}

void Mesh::draw(VkCommandBuffer cmd) const
{
    if (m_indexCount > 0) {
        vkCmdDrawIndexed(cmd, m_indexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(cmd, m_vertexCount, 1, 0, 0);
    }
}

} // namespace gfx