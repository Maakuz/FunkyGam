#pragma once
#include "Game/Components/Comp.h"
#include <unordered_map>

#define getLogisticsComp getComponent<LogisticsComp>
#define getSpriteComp getComponent<SpriteComp>
#define getAnimatedSpriteComp getComponent<AnimatedSpriteComp>
#define getMovementComp getComponent<MovementComp>
#define getTomeComp getComponent<TomeComp>
#define getHealthComp getComponent<HealthComp>
#define getColliderComp getComponent<ColliderComp>
#define getDamageComp getComponent<DamageComp>
#define getTransformComp getComponent<TransformComp>
#define getStatusComp getComponent<StatusComp>

class Entity
{
public:
    Entity() {};
    virtual ~Entity();

    Entity(const Entity& other);

    void operator=(const Entity& other);

    template <typename Component>
    void addComponent(Comp* comp);

    template <typename Component>
    Component* getComponent();

    template <typename Component>
    const Component* getComponent() const;

private:
    std::unordered_map<ComponentKey, Comp*> components;
};

template<typename Component>
inline void Entity::addComponent(Comp* comp)
{
    if (!components.count(Component::getStaticKey()))
        components.emplace(Component::getStaticKey(), comp);
}

template<typename Component>
inline Component* Entity::getComponent()
{
    if (components.count(Component::getStaticKey()))
        return dynamic_cast<Component*>(components.at(Component::getStaticKey()));

    else
        return nullptr;
}

template<typename Component>
inline const Component* Entity::getComponent() const
{
    if (components.count(Component::getStaticKey()))
        return dynamic_cast<const Component*>(components.at(Component::getStaticKey()));

    else
        return nullptr;
}
