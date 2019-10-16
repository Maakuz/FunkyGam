#pragma once
#include "../MovingEntity.h"
#include "Misc/Counter.h"
#include <fstream>


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

    friend std::istream& operator>>(std::istream& in, Enemy& enemy);

    virtual void update(float dt);
    void notifyEnemy(sf::Vector2f playerPos);
    void spawn(sf::Vector2f pos);

    Direction getFacingDir() const { return this->facingDir; }
    State getState() const { return this->state; }

    sf::Vector2f getEyePos() const;
    void setEyeLevel(sf::Vector2f eyeLevel) { this->eyeLevel = eyeLevel; };
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

    virtual std::istream& readSpecific(std::istream& in) = 0;

};