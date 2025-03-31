#pragma once

#include <vector>
#include <unordered_map>

#include "model.hpp"

namespace gfx
{

class ModelManager
{

public:
    ModelManager() = default;
    ~ModelManager() = default;

    void init(Device &device, BindlessManager &bindlessManager);
    void destroy();

    u32 loadModel(const std::string &filepath);

    Model *getModel(u32 id);
    Model *getModel(const std::string &path);

    void drawModel(VkCommandBuffer cmd, u32 id);

private:
    Device *m_device = nullptr;
    BindlessManager *m_bindlessManager = nullptr;

    std::unordered_map<u32, std::unique_ptr<Model>> m_models;
    std::unordered_map<std::string, u32> m_pathToID;

    u32 m_nextID = 0;
};

} // namespace gfx