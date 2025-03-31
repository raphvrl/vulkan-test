#include "model_manager.hpp"

namespace gfx
{

void ModelManager::init(Device &device, BindlessManager &bindlessManager)
{
    m_device = &device;
    m_bindlessManager = &bindlessManager;
}

void ModelManager::destroy()
{
    for (auto &model : m_models) {
        model.second->destroy();
    }
    m_models.clear();
}

u32 ModelManager::loadModel(const std::string &filepath)
{
    auto it = m_pathToID.find(filepath);
    if (it != m_pathToID.end()) {
        return it->second;
    }

    auto model = std::make_unique<Model>();
    model->load(*m_device, *m_bindlessManager, filepath);

    u32 id = m_nextID++;
    m_models[id] = std::move(model);
    m_pathToID[filepath] = id;

    return id;
}

Model *ModelManager::getModel(u32 id)
{
    auto it = m_models.find(id);
    if (it != m_models.end()) {
        return it->second.get();
    }
    return nullptr;
}

Model *ModelManager::getModel(const std::string &path)
{
    auto it = m_pathToID.find(path);
    if (it != m_pathToID.end()) {
        return getModel(it->second);
    }
    return nullptr;
}

void ModelManager::drawModel(VkCommandBuffer cmd, u32 id)
{
    auto model = getModel(id);
    if (model) {
        model->draw(cmd);
    }
}

} // namespace gfx