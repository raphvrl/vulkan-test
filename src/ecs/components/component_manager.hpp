#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>

#include "ecs/types.hpp"

namespace ecs
{

class IComponentArray
{

public:
    virtual ~IComponentArray() = default;
    virtual void destroyEntity(EntityID entity) = 0;

};

template <typename T>
class ComponentArray : public IComponentArray
{

public:
    void insertData(EntityID entity, T component)
    {
        if (m_entityToIndex.find(entity) == m_entityToIndex.end()) {
            m_entityToIndex[entity] = m_components.size();
            m_indexToEntity[m_components.size()] = entity;
            m_components.push_back(component);
        } else {
            m_components[m_entityToIndex[entity]] = component;
        }
    }

    void removeData(EntityID entity)
    {
        if (m_entityToIndex.find(entity) != m_entityToIndex.end()) {
            usize indexOfRemoved = m_entityToIndex[entity];
            usize indexOfLast = m_components.size() - 1;
            m_components[indexOfRemoved] = m_components[indexOfLast];

            EntityID lastEntity = m_indexToEntity[indexOfLast];
            m_entityToIndex[lastEntity] = indexOfRemoved;
            m_indexToEntity[indexOfRemoved] = lastEntity;

            m_entityToIndex.erase(entity);
            m_indexToEntity.erase(indexOfLast);
            m_components.pop_back();
        }
    }

    T &getData(EntityID entity)
    {
        if (m_entityToIndex.find(entity) != m_entityToIndex.end()) {
            return m_components[m_entityToIndex[entity]];
        } else {
            throw std::runtime_error("Component not found for entity");
        }
    }

    bool hasData(EntityID entity) const
    {
        return m_entityToIndex.find(entity) != m_entityToIndex.end();
    }

    void destroyEntity(EntityID entity) override
    {
        removeData(entity);
    }

private:
    std::vector<T> m_components;

    std::unordered_map<EntityID, usize> m_entityToIndex;
    std::unordered_map<usize, EntityID> m_indexToEntity;

};

class ComponentManager
{

public:
    template<typename T>
    void registerComponent()
    {
        std::type_index typeIndex = typeid(T);
        if (m_componentTypes.find(typeIndex) == m_componentTypes.end()) {
            m_componentTypes[typeIndex] = m_nextComponentType;
            m_componentArrays[m_nextComponentType] = std::make_unique<ComponentArray<T>>();
            ++m_nextComponentType;
        } else {
            throw std::runtime_error("Component type already registered");
        }
    }

    template<typename T>
    ComponentID getComponentType() const
    {
        std::type_index typeIndex = typeid(T);
        if (m_componentTypes.find(typeIndex) != m_componentTypes.end()) {
            return m_componentTypes.at(typeIndex);
        } else {
            throw std::runtime_error("Component type not registered");
        }
    }

    template<typename T>
    void addComponent(EntityID entity, T component)
    {
        getComponentArray<T>()->insertData(entity, component);
    }

    template<typename T>
    void removeComponent(EntityID entity)
    {
        getComponentArray<T>()->removeData(entity);
    }

    template<typename T>
    T &getComponent(EntityID entity)
    {
        return getComponentArray<T>()->getData(entity);
    }

    template<typename T>
    bool hasComponent(EntityID entity) const
    {
        return getComponentArray<T>()->hasData(entity);
    }

    void entityDestroyed(EntityID entity)
    {
        for (auto const &[type, array] : m_componentArrays) {
            array->destroyEntity(entity);
        }
    }

private:
    std::unordered_map<std::type_index, ComponentID> m_componentTypes;
    std::unordered_map<ComponentID, std::unique_ptr<IComponentArray>> m_componentArrays;

    ComponentID m_nextComponentType = 0;

    template<typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray()
    {
        const char *typeName = typeid(T).name();
        if (m_componentTypes.find(typeName) != m_componentTypes.end()) {
            return std::static_pointer_cast<ComponentArray<T>>(
                m_componentArrays[m_componentTypes[typeName]]
            );
        } else {
            throw std::runtime_error("Component type not registered");
        }
    }
};

} // namespace ecs