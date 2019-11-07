#pragma once
#include "../MovingEntity.h"
#include "Game/Misc/Counter.h"
#include "Game/Interface/UIHandler.h"
#include <fstream>


class Enemy : public MovingEntity
{
public:
    enum class Direction
    {
        left,
        right,
        up,
        down,
        none
    };

    enum class State
    {
        idle,
        chasing,
        attacking,
        searching,
        returning,
        stunned
    };

    Enemy(AnimationData data, sf::Vector2f pos, UIHandler* ui);
    ~Enemy() {};

    friend std::istream& operator>>(std::istream& in, Enemy& enemy);

    virtual void update(float dt);
    void notifyEnemy(sf::Vector2f playerPos);
    void spawn(sf::Vector2f pos);

    bool isAlive();

    Direction getFacingDir() const { return this->facingDir; }
    State getState() const { return this->state; }
    float getSightRadius() const { return this->sightRadius; }
    float getVisionRatingAt(float distance) const;

    sf::Vector2f getEyePos() const;
    void setEyeLevel(sf::Vector2f eyeLevel) { this->eyeLevel = eyeLevel; };
    sf::Vector2f getLastKnownPos() const { return this->lastKnownPlayerPos; };

protected:
    UIHandler* ui;
    float roamDistance;
    State state;
    Direction facingDir;
    Direction forcedDirection;
    sf::Vector2f eyeLevel;
    Counter timeSincePlayerSeen;
    sf::Vector2f currentRoamPoint;
    int health;
    int maxHealth;
    float chaseSpeed;
    float idleSpeed;
    float sightRadius;
    float sightMultiplier; //How well enemy sees in the dark
    Counter drawQuestion;
    Counter stunCounter;
    Counter drawExclamation;
    sf::Sprite question;
    sf::Sprite exclamation;

    void moveLeft();
    void moveRight();

    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }
    void setStartPoint(sf::Vector2f point) { this->startPoint = point; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    Counter roamDecisionCounter;
    bool decisionTime;
    int prevHealth;
    sf::Vector2f startPoint;
    sf::Vector2f lastKnownPlayerPos;

    virtual std::istream& readSpecific(std::istream& in) = 0;

};