#pragma once
#include "AnimatedEntity.h"

class MovingEntity : public AnimatedEntity
{
public:
    MovingEntity(AnimationData data, sf::Vector2f pos);
    ~MovingEntity() {};

    virtual void update(float dt);

protected:
    void jump();
    //momentum.y = momentum.y * haltforce 
    void stopJump(float haltForce = 0);
    virtual void handleCollision(const Entity& collider);

    sf::Vector2i acceleration;
    sf::Vector2f momentum;

    float walkSpeed;
    float jumpHeight;
    float mass;

    bool jumping;
    bool grounded;

private:
    float floorRes;
    float airRes;
    float airMobility;
    
};