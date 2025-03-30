#pragma once

#include <tiny_gltf.h>

#include <memory>

#include "device.hpp"
#include "mesh.hpp"
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

    std::vector<std::unique_ptr<Mesh>> m_meshes;

    void processMeshes(const tinygltf::Model &gltfModel);

};

} // namespace gfx