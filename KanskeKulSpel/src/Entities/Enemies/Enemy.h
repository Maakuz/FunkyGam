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
    enum class State
    {
        idle,
        chasing,
        returning
    };

    float roamDistance;
    State state;

    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }

private:
    Counter roamDecisionCounter;
    bool decisionTime;
    sf::Vector2f startPoint;
};