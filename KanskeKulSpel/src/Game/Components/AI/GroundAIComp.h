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

    void updateIdle(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateChasing(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateReturn(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateSearch(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateStunned(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);



    virtual void handleCollision(MovementComp* movement, ColliderComp* collider, const ColliderComp* otherCollidable);
private:

};