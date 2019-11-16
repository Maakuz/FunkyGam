#pragma once

#include "Game/Entities/Enemies/Enemy.h"

class FishMonger
{
public:
    FishMonger();
    ~FishMonger();

    virtual void update(float dt);

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion);
private:
    virtual std::istream& readSpecific(std::istream& in);
};