#pragma once
#include "Game/Entities/Entity.h"

class Terrain : public Entity
{
public:
    Terrain(Collider::AABB aabb, Collider::ColliderKeys type, std::string colliderFlag = "");
    ~Terrain() {};

    virtual void handleCollision(const Entity* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};

private:


};