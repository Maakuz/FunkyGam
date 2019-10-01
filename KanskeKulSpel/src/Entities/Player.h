#pragma once
#include "AnimatedEntity.h"
#include "Misc/Counter.h"

class Player : public AnimatedEntity 
{
public:
    Player(AnimationData data, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    void update(float dt);

    virtual void handleCollision(const Entity&  collider);

private:
    void move(float dt);
    void debugMove(float dt);
    void jump();

    sf::Vector2i acceleration;
    sf::Vector2f momentum;

    float lightBounceFreq;
    float lightBounceHeight;

    float walkSpeed;
    float floorRes;
    float jumpHeight;
    float mass;

    bool grounded;
    Counter platformPassingCounter;

};