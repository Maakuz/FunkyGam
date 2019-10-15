#pragma once
#include "MovingEntity.h"
#include "Misc/Counter.h"

class Player : public MovingEntity 
{
public:
    Player(AnimationData data, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    void update(float dt, sf::Vector2f mousePos);

    virtual void handleCollision(const Entity&  collider);

private:
    Counter platformPassingCounter;
    
    void move(float dt);
    void debugMove(float dt);

    bool debugMode;
};