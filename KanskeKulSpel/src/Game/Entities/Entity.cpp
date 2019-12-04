#include "Entity.h"

Entity::~Entity()
{
    for (auto& pair : components)
        delete pair.second;
}

Entity::Entity(const Entity& other)
{
    *this = other;
}

void Entity::operator=(const Entity& other)
{
    for (auto& pair : this->components)
        delete pair.second;

    components.clear();

    for (auto& component : other.components)
        this->components.emplace(component.first, component.second->clone());
}

