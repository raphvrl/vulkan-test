#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <set>

#include "ecs/types.hpp"

namespace ecs
{

class ECS;

class System
{

public:
    System(ECS *ecs) : m_ecs(ecs) {}
    virtual ~System() = default;
    virtual void tick(f32 dt) = 0;

protected:
    ECS *m_ecs = nullptr;

};

class SystemManager
{

public:
    template<typename T>
    std::shared_ptr<T> registerSystem(ECS *ecs)
    {
        std::type_index typeIndex = typeid(T);
        if (m_systems.find(typeIndex) == m_systems.end()) {
            auto system = std::make_shared<T>(ecs);
            m_systems[typeIndex] = system;
            return system;
        } else {
            throw std::runtime_error("System already registered");
        }
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> registerSystem(Args &&... args)
    {
        const char *typeName = typeid(T).name();
        if (m_systems.find(typeName) == m_systems.end()) {
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            m_systems[typeName] = system;
            return system;
        } else {
            throw std::runtime_error("System already registered");
        }
    }

    template<typename T>
    void setSignature(Signature signature)
    {
        std::type_index typeIndex = typeid(T);
        if (m_systems.find(typeIndex) != m_systems.end()) {
            m_systemSignatures[typeIndex] = signature;
        } else {
            throw std::runtime_error("System not registered");
        }
    }

    void entityDestroyed(EntityID entity)
    {
        for (auto const &[type, system] : m_systems) {
            system->m_entities.erase(entity);
        }
    }

    void entitySignatureChanged(EntityID entity, Signature entitySignature)
    {
        for (auto const &[type, system] : m_systems) {
            Signature systemSignature = m_systemSignatures[type];
            if ((entitySignature & systemSignature) == systemSignature) {
                system->m_entities.insert(entity);
            } else {
                system->m_entities.erase(entity);
            }
        }
    }

    template<typename T>
    std::shared_ptr<T> getSystem()
    {
        std::type_index typeIndex = typeid(T);
        if (m_systems.find(typeIndex) != m_systems.end()) {
            return std::static_pointer_cast<T>(m_systems[typeIndex]);
        } else {
            throw std::runtime_error("System not registered");
        }
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;
    std::unordered_map<std::type_index, Signature> m_systemSignatures;

};

} // namespace ecs