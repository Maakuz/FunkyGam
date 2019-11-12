#pragma once
#include "AnimatedEntity.h"

class MovingEntity : public AnimatedEntity
{
public:
    MovingEntity(AnimationData data, sf::Vector2f pos);
    ~MovingEntity() {};

    virtual void update(float dt);
    sf::Vector2f getMomentum()const { return momentum; };
    float getMass()const { return this->mass; };

protected:
    void setIgnoreGravity(bool ignoringGravity) { this->ignoringGravity = ignoringGravity; };
    void jump();
    //momentum.y = momentum.y * haltforce 
    void stopJump(float haltForce = 0);
    virtual void handleCollision(const Entity* collider);
    void addCollisionMomentum(sf::Vector2f colliderMomentum, sf::Vector2f colliderPos, float colliderMass);

    sf::Vector2f acceleration;
    sf::Vector2f momentum;

    bool addedMomentum;
    sf::Vector2f collisionMomentum;

    float walkSpeed;
    float jumpHeight;
    float mass;

    bool jumping;
    bool grounded;

private:
    bool ignoringGravity;
    float floorRes;
    float airRes;
    float airMobility;
    
};