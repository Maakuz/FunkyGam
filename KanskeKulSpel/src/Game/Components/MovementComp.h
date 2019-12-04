#pragma once
#include "SFML/System/Vector2.hpp"
#include "Comp.h"
#include "TransformComp.h"
#include <istream>

class MovementComp : public Comp
{
public:
    MovementComp();
    virtual ~MovementComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::movement; };
    Comp* clone() const { return new MovementComp(*this); };

    void reset();

    virtual void update(float dt);

    void jump();
    //momentum.y = momentum.y * haltforce 
    void stopJump(float haltForce = 0);
    void addCollisionMomentum(sf::Vector2f momentum);

    void setIgnoreGravity(bool ignoringGravity) { this->ignoringGravity = ignoringGravity; };
    void setAirRes(float airRes) { this->airRes = airRes; };

    bool isJumping() const { return jumping; };

    TransformComp transform;
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