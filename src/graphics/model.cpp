#include "model.hpp"

namespace gfx
{

void Model::load(
    Device &device,
    BindlessManager &bindlessManager,
    const std::string &filepath
)
{
    m_device = &device;
    m_bindlessManager = &bindlessManager;

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (filepath.find(".gltf") != std::string::npos) {
        if (!loader.LoadASCIIFromFile(&gltfModel, &err, &warn, filepath)) {
            throw std::runtime_error("Failed to load glTF model: " + err);
        }
    } else {
        throw std::runtime_error("Unsupported file format: " + filepath);
    }

    if (!warn.empty()) {
        std::cout << "GLTF Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "GLTF Error: " << err << std::endl;
    }

    std::vector<u32> textureIDs;
    processTextures(gltfModel, textureIDs);

    processMeshes(gltfModel, textureIDs);
}

void Model::destroy()
{
    for (auto &mesh : m_meshes) {
        mesh.destroy();
    }

    m_meshes.clear();

    for (auto &texture : m_textures) {
        texture.destroy();
    }

    m_textures.clear();
}

void Model::draw(VkCommandBuffer cmd)
{
    for (auto &mesh : m_meshes) {
        mesh.bind(cmd);
        mesh.draw(cmd);
    }
}

void Model::processMeshes(
    const tinygltf::Model &gltfModel,
    const std::vector<u32> &textureIDs
)
{
    for (const auto &mesh : gltfModel.meshes) {
        for (const auto &primitive : mesh.primitives) {
            std::vector<Mesh::Vertex> vertices;
            std::vector<u32> indices;

            usize vertexCount = 0;

            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor &posAccessor = 
                    gltfModel.accessors[primitive.attributes.at("POSITION")];

                vertexCount = posAccessor.count;
                vertices.resize(vertexCount);
            } else {
                throw std::runtime_error("Mesh has no POSITION attribute.");
            }

            if (primitive.indices >= 0) {
                const tinygltf::Accessor &accessor = gltfModel.accessors[primitive.indices];
                const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
            
                indices.resize(accessor.count);
                const void* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;
                
                switch (accessor.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                        const uint16_t* typedIndices =
                            reinterpret_cast<const uint16_t*>(data);
                        for (usize i = 0; i < accessor.count; i++) {
                            indices[i] = static_cast<u32>(typedIndices[i]);
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                        const uint32_t* typedIndices =
                            reinterpret_cast<const uint32_t*>(data);
                        for (usize i = 0; i < accessor.count; i++) {
                            indices[i] = typedIndices[i];
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                        const uint8_t* typedIndices = 
                            reinterpret_cast<const uint8_t*>(data);
                        for (usize i = 0; i < accessor.count; i++) {
                            indices[i] = static_cast<u32>(typedIndices[i]);
                        }
                        break;
                    }
                    default:
                        std::cerr << "Unsupported index component type: " << accessor.componentType << std::endl;
                        indices.resize(vertexCount);
                        for (usize i = 0; i < vertexCount; i++) {
                            indices[i] = static_cast<u32>(i);
                        }
                }
            } else {
                indices.resize(vertexCount);
                for (usize i = 0; i < vertexCount; i++) {
                    indices[i] = static_cast<u32>(i);
                }
            }

            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor &accessor = 
                    gltfModel.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView &bufferView = 
                    gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer =
                    gltfModel.buffers[bufferView.buffer];

                const f32 *data = reinterpret_cast<const f32 *>(
                    buffer.data.data() + accessor.byteOffset + bufferView.byteOffset
                );

                for (usize i = 0; i < vertexCount; ++i) {
                    vertices[i].pos = glm::vec3(
                        data[i * 3],
                        data[i * 3 + 1],
                        data[i * 3 + 2]
                    );
                }
            } else {
                throw std::runtime_error("Mesh has no POSITION attribute.");
            }

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor &accessor = 
                    gltfModel.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView &bufferView = 
                    gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer =
                    gltfModel.buffers[bufferView.buffer];

                const f32 *data = reinterpret_cast<const f32 *>(
                    buffer.data.data() + accessor.byteOffset + bufferView.byteOffset
                );

                for (usize i = 0; i < vertexCount; ++i) {
                    vertices[i].normal = glm::vec3(
                        data[i * 3],
                        data[i * 3 + 1],
                        data[i * 3 + 2]
                    );
                }
            } else {
                for (usize i = 0; i < vertexCount; ++i) {
                    vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
                }
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor &accessor = 
                    gltfModel.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView &bufferView = 
                    gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer =
                    gltfModel.buffers[bufferView.buffer];

                const f32 *data = reinterpret_cast<const f32 *>(
                    buffer.data.data() + accessor.byteOffset + bufferView.byteOffset
                );

                for (usize i = 0; i < vertexCount; ++i) {
                    vertices[i].uv = glm::vec2(
                        data[i * 2],
                        data[i * 2 + 1]
                    );
                }
            } else {
                for (usize i = 0; i < vertexCount; ++i) {
                    vertices[i].uv = glm::vec2(0.0f, 0.0f);
                }
            }

            u32 textureID = 0;
            if (
                primitive.material >= 0 &&
                primitive.material < static_cast<int>(gltfModel.materials.size())
            ) {
                const auto& material = gltfModel.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    int texIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                    textureID = textureIDs[texIndex + 1];
                }
            }

            Mesh mesh;
            mesh.init(*m_device, vertices, indices);
            m_meshes.push_back(std::move(mesh));
            m_meshes.back().setTextureID(textureID);
        }
    }
}

void Model::processTextures(
    const tinygltf::Model &gltfModel,
    std::vector<u32> &textureIDs
)
{
    textureIDs.resize(gltfModel.textures.size() + 1, 0);

    Image defaultImage;
    u32 whitePixel = 0xFFFFFFFF;
    defaultImage.init(
        *m_device,
        &whitePixel,
        1,
        1,
        VK_FORMAT_R8G8B8A8_UNORM
    );

    textureIDs[0] = m_bindlessManager->addTexture(defaultImage);
    m_textures.push_back(std::move(defaultImage));

    for (usize i = 0; i < gltfModel.textures.size(); i++) {
        const tinygltf::Texture &gltfTexture = gltfModel.textures[i];
        
        if (
            gltfTexture.source < 0 ||
            gltfTexture.source >= static_cast<int>(gltfModel.images.size())
        ) {
            throw std::runtime_error("Invalid texture source index.");
        }

        const tinygltf::Image &image = gltfModel.images[gltfTexture.source];
        Image textureImage;

        if (image.component == 4) {
            textureImage.init(
                *m_device,
                image.image.data(),
                image.width,
                image.height,
                VK_FORMAT_R8G8B8A8_UNORM
            );
        } else if (image.component == 3) {
            textureImage.init(
                *m_device,
                image.image.data(),
                image.width,
                image.height,
                VK_FORMAT_R8G8B8_UNORM
            );
        } else {
            throw std::runtime_error("Unsupported image format.");
        }

        textureIDs[i + 1] = m_bindlessManager->addTexture(textureImage);
        m_textures.push_back(std::move(textureImage));
    }
}

} // namespace gfx