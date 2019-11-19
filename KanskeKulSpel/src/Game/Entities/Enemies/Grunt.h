#pragma once
#include "Enemy.h"
#include "Game/Components/AI/GroundAIComp.h"

class Grunt : public Enemy
{
public:
    Grunt(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    ~Grunt() {};

    void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);

    int getDamage()const { return this->damage; };
    bool isAttacking()const { return this->flying; };
    
private:
    int damage;
    sf::Vector2f attackMomentum;
    bool flying;
    GroundAIComp ai;

    virtual const AIComp* getAI() const { return &ai; };
    virtual AIComp* getAI() { return &ai; };

    void updateAttack(float dt);
    virtual std::istream& readSpecific(std::istream& in);
};