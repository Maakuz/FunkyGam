#pragma once
#include "Misc/LuaScript.h"
#include "Game/Entities/Enemies/Enemy.h"
#include "Game/Components/AnimatedSpriteComp.h"

class FishMonger : public Enemy
{
public:
    FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    ~FishMonger() {};

    virtual void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);
private:
    virtual std::istream& readSpecific(std::istream& in);

    //LuaScript ai;
};