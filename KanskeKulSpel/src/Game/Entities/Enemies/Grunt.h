#pragma once
#include "Enemy.h"

class Grunt : public Enemy
{
public:
    Grunt(AnimationData data, sf::Vector2f pos, UIHandler* ui);
    ~Grunt() {};

    void update(float dt);

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion);

    int getDamage()const { return this->damage; };
    bool isAttacking()const { return this->flying; };

private:
    float attackDistance;
    int damage;
    sf::Vector2f attackMomentum;
    Counter attackChargeTimer;
    Counter searchCounter;
    bool flying;



    void updateIdle(float dt);
    void updateChasing(float dt);
    void updateReturning(float dt);
    void updateAttack(float dt);
    virtual std::istream& readSpecific(std::istream& in);
};