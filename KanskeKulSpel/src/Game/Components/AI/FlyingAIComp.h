#pragma once
#include "AIComp.h"
#include "Game/Entities/Collidable.h"

class FlyingAIComp : public AIComp
{
public:
    FlyingAIComp(sf::Vector2f pos, sf::Vector2f size);
    virtual ~FlyingAIComp() {};

    void updateIdle(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateChasing(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateReturn(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);
    void updateStunned(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite = nullptr);

    Counter attackCounter;
    sf::Vector2f attackDestination;

    virtual void handleCollision(MovementComp* movement, ColliderComp* collider, const ColliderComp* otherCollidable, SpriteComp* sprite = nullptr);
private:
    bool forcedDirResolved;

};
