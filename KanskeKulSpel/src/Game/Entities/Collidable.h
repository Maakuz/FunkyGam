#pragma once
#include "Game/Components/ColliderComp.h"
#include "Game/Entities/Entity.h"

class Collidable : public Entity
{
public:
    Collidable(sf::Vector2f pos = sf::Vector2f(), sf::Vector2f size = sf::Vector2f())
    {
        ColliderComp* comp = new ColliderComp(size, pos);

        addComponent<ColliderComp>(comp);
    };

    virtual ~Collidable() {};

protected:
    friend class CollisionHandler;

    virtual void handleCollision(const Collidable* collidable) = 0;
    virtual void handleExplosion(const Explosion& explosion) = 0;
};