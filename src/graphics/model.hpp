#pragma once

#include <tiny_gltf.h>

#include <memory>

#include "device.hpp"
#include "mesh.hpp"
#include "image.hpp"
#include "bindless_manager.hpp"

namespace gfx
{

class Model
{

public:
    Model() = default;
    ~Model() = default;

    void load(
        Device &device,
        BindlessManager &bindlessManager,
        const std::string &filepath
    );

    void destroy();

    void draw(VkCommandBuffer cmd);

private:
    Device *m_device = nullptr;
    BindlessManager *m_bindlessManager = nullptr;

    std::vector<Mesh> m_meshes;
    std::vector<Image> m_textures;

    void processMeshes(
        const tinygltf::Model &gltfModel,
        const std::vector<u32> &textureIDs
    );

    void processTextures(
        const tinygltf::Model &gltfModel,
        std::vector<u32> &textureIDs
    );

};

} // namespace gfx