#pragma once
#include "Entity.h"

class Terrain : public Entity
{
public:
    Terrain(CollisionBox::AABB aabb, CollisionBox::ColliderKeys type);
    ~Terrain() {};

    virtual void handleCollision(const Entity* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};

private:


};