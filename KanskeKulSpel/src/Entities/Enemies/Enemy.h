#pragma once
#include "../MovingEntity.h"
#include "Misc/Counter.h"

class Enemy : public MovingEntity
{
public:
    enum class Direction
    {
        left,
        right,
        none
    };

    enum class State
    {
        idle,
        chasing,
        attacking,
        returning,
        stunned
    };

    Enemy(AnimationData data, sf::Vector2f pos);
    ~Enemy() {};

    virtual void update(float dt);
    void notifyEnemy(sf::Vector2f playerPos);


    Direction getFacingDir() const { return this->facingDir; }
    State getState() const { return this->state; }

    sf::Vector2f getEyePos() const;
    sf::Vector2f getLastKnownPos() const { return this->lastKnownPlayerPos; };

protected:
    float roamDistance;
    State state;
    Direction facingDir;
    sf::Vector2f eyeLevel;
    Counter timeSincePlayerSeen;

    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }
    void setStartPoint(sf::Vector2f point) { this->startPoint = point; }
private:
    Counter roamDecisionCounter;
    bool decisionTime;
    sf::Vector2f startPoint;
    sf::Vector2f lastKnownPlayerPos;
};