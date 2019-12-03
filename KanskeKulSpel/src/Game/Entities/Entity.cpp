#include "Entity.h"

Entity::~Entity()
{
    for (auto& pair : components)
        delete pair.second;
}

void Entity::addComponent(Comp* comp)
{
    components.emplace(comp->getKey(), comp);
}
