#pragma once
#include "Game/Entities/Collidable.h"

class Terrain : public Collidable
{
public:
    Terrain(AABB aabb, ColliderKeys type, std::string colliderFlag = "");
    ~Terrain() {};

    virtual void handleCollision(const Collidable* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};
    virtual const ColliderComp& getCollider()const { return collider; };

private:
    ColliderComp collider;
};