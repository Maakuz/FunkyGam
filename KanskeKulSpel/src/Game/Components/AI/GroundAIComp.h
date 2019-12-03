#pragma once
#include "AIComp.h"
#include "Game/Entities/Collidable.h"

class GroundAIComp : public AIComp
{
public:
    Counter searchCounter;
    float attackDistance;
    Counter attackChargeTimer;

    GroundAIComp(sf::Vector2f pos, sf::Vector2f size);
    virtual ~GroundAIComp() {};

    void updateIdle(float dt, SpriteComp* sprite = nullptr);
    void updateChasing(float dt, SpriteComp* sprite = nullptr);
    void updateReturn(float dt, SpriteComp* sprite = nullptr);
    void updateSearch(float dt, SpriteComp* sprite = nullptr);
    void updateStunned(float dt, SpriteComp* sprite = nullptr);



    virtual void handleCollision(const ColliderComp* collidable);
private:

};