#pragma once
#include "Game/Components/ColliderComp.h"

class Collidable
{
public:
    virtual const ColliderComp& getCollider()const = 0;

protected:
    friend class CollisionHandler;

    virtual void handleCollision(const Collidable* collidable) = 0;
    virtual void handleExplosion(const Explosion& explosion) = 0;
};