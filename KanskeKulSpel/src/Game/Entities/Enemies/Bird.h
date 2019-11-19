#pragma once
#include "Enemy.h"
#include "Game/Components/AI/FlyingAIComp.h"

class Bird : public Enemy
{
public:
    Bird(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    ~Bird();

    void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);

private:
    bool inBombingRange;
    bool startBombing;
    Counter bombsPerAttack;
    Counter bombCounter;
    FlyingAIComp ai;

    void updateAttack(float dt);

    virtual const AIComp* getAI() const { return &ai; };
    virtual AIComp* getAI() { return &ai; };
    
    virtual std::istream& readSpecific(std::istream& in);
};