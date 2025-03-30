#include "entity_manager.hpp"

namespace ecs
{

EntityManager::EntityManager()
{
    m_actives.fill(false);
}

EntityID EntityManager::createEntity()
{
    EntityID id;
    if (!m_availables.empty()) {
        id = m_availables.front();
        m_availables.pop();
    } else {
        if (m_nextEntityID >= MAX_ENTITIES) {
            throw std::runtime_error("Maximum number of entities reached");
        }
        id = m_nextEntityID++;
    }

    m_actives[id] = true;
    return id;
}

void EntityManager::destroyEntity(EntityID id)
{
    if (id >= m_signatures.size() || !m_actives[id]) {
        return;
    }

    m_actives[id] = false;
    m_signatures[id].reset();
    m_availables.push(id);
}

bool EntityManager::isEntityActive(EntityID id) const
{
    return id < m_actives.size() && m_actives[id];
}

void EntityManager::setSignature(EntityID id, Signature signature)
{
    if (id >= m_signatures.size()) {
        return;
    }

    m_signatures[id] = signature;
}

Signature EntityManager::getSignature(EntityID id) const
{
    if (id >= m_signatures.size()) {
        return Signature();
    }

    return m_signatures[id];
}

void EntityManager::setComponent(
    EntityID id,
    ComponentID component,
    bool hasComponent
)
{
    if (id >= m_signatures.size()) {
        return;
    }

    m_signatures[id].set(component, hasComponent);
}

std::vector<EntityID> EntityManager::getActiveEntities() const
{
    std::vector<EntityID> activeEntities;
    activeEntities.reserve(MAX_ENTITIES - m_availables.size());
    
    for (EntityID id = 0; id < m_actives.size(); ++id) {
        if (m_actives[id]) {
            activeEntities.push_back(id);
        }
    }

    return activeEntities;
}

usize EntityManager::getActiveEntityCount() const
{
    return MAX_ENTITIES - m_availables.size();
}

} // namespace ecs