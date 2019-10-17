#pragma once
#include "Enemy.h"

class Grunt : public Enemy
{
public:
    Grunt(AnimationData data, sf::Vector2f pos);
    ~Grunt() {};

    void update(float dt);

    virtual void handleCollision(const Entity& collider);
    virtual void handleExplosion(const Explosion& explosion);

private:
    Direction forcedDirection;
    float chaseSpeed;
    float idleSpeed;
    float attackDistance;
    sf::Vector2f attackMomentum;
    Counter attackChargeTimer;
    Counter stunCounter;
    Counter searchCounter;
    bool flying;


    void moveLeft();
    void moveRight();
    void updateIdle(float dt);
    void updateChasing(float dt);
    void updateReturning(float dt);
    void updateAttack(float dt);
    virtual std::istream& readSpecific(std::istream& in);
};