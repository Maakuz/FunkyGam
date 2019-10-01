#pragma once
#include "Entity.h"

class Terrain : public Entity
{
public:
    Terrain(CollisionBox::AABB aabb, CollisionBox::colliderComponents type);
    ~Terrain() {};

    virtual void handleCollision(const Entity& collider) {};

private:


};