#pragma once

#include "entity_manager.hpp"
#include "component_manager.hpp"
#include "system_manager.hpp"

namespace ecs
{

class ECS
{

public:
    void init()
    {
        m_entityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_systemManager = std::make_unique<SystemManager>();
    }

    EntityID createEntity()
    {
        return m_entityManager->createEntity();
    }

    void destroyEntity(EntityID entity)
    {
        m_entityManager->destroyEntity(entity);
        m_componentManager->entityDestroyed(entity);
        m_systemManager->entityDestroyed(entity);
    }

    template<typename T>
    void registerComponent()
    {
        m_componentManager->registerComponent<T>();
    }

    template<typename T>
    void addComponent(EntityID entity, T component)
    {
        m_componentManager->addComponent<T>(entity, component);
        
        auto signature = m_entityManager->getSignature(entity);
        signature.set(m_componentManager->getComponentID<T>(), true);
        m_entityManager->setSignature(entity, signature);

        m_systemManager->entitySignatureChanged(entity, signature);
    }

    template<typename T>
    void removeComponent(EntityID entity)
    {
        m_componentManager->removeComponent<T>(entity);

        auto signature = m_entityManager->getSignature(entity);
        signature.set(m_componentManager->getComponentID<T>(), false);
        m_entityManager->setSignature(entity, signature);

        m_systemManager->entitySignatureChanged(entity, signature);
    }

    T &getComponent(EntityID entity)
    {
        return m_componentManager->getComponent<T>(entity);
    }

    template<typename T>
    bool hasComponent(EntityID entity)
    {
        return m_componentManager->hasComponent<T>(entity);
    }

    template<typename T>
    ComponentID getComponentID()
    {
        return m_componentManager->getComponentID<T>();
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> registerSystem(Args &&... args)
    {
        return m_systemManager->registerSystem<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    void setSystemSignature(Signature signature)
    {
        m_systemManager->setSignature<T>(signature);
    }

private:
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<SystemManager> m_systemManager;

};

} // namespace ecs