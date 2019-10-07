#pragma once
#include "../MovingEntity.h"
#include "Misc/Counter.h"

class Enemy : public MovingEntity
{
public:
    Enemy(AnimationData data, sf::Vector2f pos);
    ~Enemy() {};

    virtual void update(float dt);

protected:
    float roamDistance;
    Counter roamDecisionCounter;

    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }

private:
    bool decisionTime;
    sf::Vector2f startPoint;
};