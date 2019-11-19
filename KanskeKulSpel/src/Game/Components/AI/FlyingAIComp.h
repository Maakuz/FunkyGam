#pragma once
#include "AIComp.h"
#include "Game/Entities/Collidable.h"

class FlyingAIComp : public AIComp
{
public:
    FlyingAIComp(sf::Vector2f pos, sf::Vector2f size);
    virtual ~FlyingAIComp() {};

    void updateIdle(float dt, SpriteComp* sprite = nullptr);
    void updateChasing(float dt, SpriteComp* sprite = nullptr);
    void updateReturn(float dt, SpriteComp* sprite = nullptr);
    void updateStunned(float dt, SpriteComp* sprite = nullptr);

    Counter attackCounter;
    sf::Vector2f attackDestination;

    virtual void handleCollision(const Collidable* collidable, SpriteComp* sprite = nullptr);
private:
    bool forcedDirResolved;

};
