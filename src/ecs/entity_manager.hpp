#pragma once

#include <queue>
#include <array>
#include <stdexcept>

#include "types.hpp"

namespace ecs
{

class EntityManager
{

public:
    EntityManager();
    ~EntityManager() = default;

    EntityID createEntity();
    void destroyEntity(EntityID entity);

    bool isEntityActive(EntityID entity) const;
    void setSignature(EntityID entity, Signature signature);

    Signature getSignature(EntityID entity) const;
    void setComponent(EntityID entity, ComponentID component, bool hasComponent);

    std::vector<EntityID> getActiveEntities() const;
    usize getActiveEntityCount() const;

private:
    EntityID m_nextEntityID = 0;

    std::queue<EntityID> m_availables;
    std::array<Signature, MAX_ENTITIES> m_signatures;
    std::array<bool, MAX_ENTITIES> m_actives;
};

} // namespace ecs