#pragma once
#include "AnimatedEntity.h"
#include "Misc/Counter.h"

class MovingEntity : public AnimatedEntity
{
public:
    MovingEntity(AnimationData data, sf::Vector2f pos);
    ~MovingEntity() {};

protected:
    void jump();
    virtual void handleCollision(const Entity& collider);

    sf::Vector2i acceleration;
    sf::Vector2f momentum;

    float walkSpeed;
    float floorRes;
    float jumpHeight;
    float mass;

    bool grounded;
    Counter platformPassingCounter;

};