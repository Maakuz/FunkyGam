#pragma once
#include "Misc/LuaScript.h"
#include "Game/Entities/Enemies/Bosses/Boss.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/AI/GroundAIComp.h"

class FishMonger : public Boss
{
public:
    FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    ~FishMonger() {};

    virtual void update(float dt);
    virtual bool isInterrupting() { return true; };

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);
private:
    virtual std::istream& readSpecific(std::istream& in);
    GroundAIComp groundMove;
    //LuaScript ai;

    virtual const AIComp* getAI() const { return &groundMove; };
    virtual AIComp* getAI() { return &groundMove; };
};