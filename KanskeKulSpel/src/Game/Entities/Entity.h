#pragma once
#include "Game/Components/Comp.h"
#include <unordered_map>

class Entity
{
public:
    Entity() {};
    ~Entity();

    void addComponent(Comp* comp);

    template <typename Component>
    Component* getComponent();

    template <typename Component>
    const Component* getComponent() const;

private:
    std::unordered_map<ComponentKey, Comp*> components;
};

template<typename Component>
inline Component* Entity::getComponent()
{
    Component comp;

    if (components.count(comp.getKey()))
        return dynamic_cast<Component*>(components.at(comp.getKey()));

    else
        return nullptr;
}

template<typename Component>
inline const Component* Entity::getComponent() const
{
    Component comp;
    if (components.count(comp.getKey()))
        return dynamic_cast<const Component*>(components.at(comp.getKey()));

    else
        return nullptr;
}
