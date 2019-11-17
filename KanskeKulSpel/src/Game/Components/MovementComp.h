#pragma once
#include "SFML/System/Vector2.hpp"
#include <istream>

class MovementComp
{
public:
    MovementComp();
    ~MovementComp() {};

    void reset();

    virtual sf::Vector2f update(float dt, sf::Vector2f pos);

    void jump();
    //momentum.y = momentum.y * haltforce 
    void stopJump(float haltForce = 0);
    void addCollisionMomentum(sf::Vector2f momentum);

    void setIgnoreGravity(bool ignoringGravity) { this->ignoringGravity = ignoringGravity; };
    void setAirRes(float airRes) { this->airRes = airRes; };

    bool isJumping() const { return jumping; };

    sf::Vector2f acceleration;
    sf::Vector2f momentum;

    float walkSpeed;
    float jumpHeight;
    float mass;
    bool grounded;

private:
    bool addedMomentum;
    sf::Vector2f collisionMomentum;

    bool jumping;

    bool ignoringGravity;
    float floorRes;
    float airRes;
    float airMobility;
};