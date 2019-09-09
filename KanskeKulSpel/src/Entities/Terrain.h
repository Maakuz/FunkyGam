#pragma once
#include "Entity.h"

class Terrain : public Entity
{
public:
    Terrain(CollisionBox::AABB aabb);
    ~Terrain() {};

    virtual void handleCollision(const Entity& collider) {};
private:

};