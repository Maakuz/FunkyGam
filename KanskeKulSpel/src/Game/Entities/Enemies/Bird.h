#pragma once
#include "Enemy.h"

class Bird : public Enemy
{
public:
    Bird(AnimationData data, sf::Vector2f pos, UIHandler* ui, sf::Vector2f size, sf::Vector2f offset);
    ~Bird();

    void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);

private:
    bool inBombingRange;
    bool forcedDirResolved;
    bool startBombing;
    Counter bombsPerAttack;
    Counter attackCounter;
    Counter bombCounter;

    sf::Vector2f attackDestination;

    void updateIdle(float dt);
    void updateChasing(float dt);
    void updateReturning(float dt);
    void updateAttack(float dt);

    virtual std::istream& readSpecific(std::istream& in);
};