#pragma once
#include "Enemy.h"

class Grunt : public Enemy
{
public:
    Grunt(AnimationData data, sf::Vector2f pos);
    ~Grunt() {};

    void update(float dt);

    virtual void handleCollision(const Entity& collider);

private:
    int boundReached;
    float chaseSpeed;
    float idleSpeed;
    float attackRange;
    Counter attackChargeTimer;


    void updateIdle(float dt);
    void updateChasing(float dt);
    void updateReturning(float dt);
    void updateAttack(float dt);
};