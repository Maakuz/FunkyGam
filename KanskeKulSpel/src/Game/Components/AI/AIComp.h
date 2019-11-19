#pragma once
#include "../MovementComp.h"
#include "../ColliderComp.h"
#include "../SpriteComp.h"
#include "Game/Misc/Counter.h"



class AIComp
{
public:
    enum class State
    {
        idle,
        chasing,
        attacking,
        searching,
        returning,
        stunned
    };

    enum class Direction
    {
        left,
        right,
        up,
        down,
        none
    };

    float chaseSpeed;
    float idleSpeed;
    float sightRadius;
    float sightMultiplier; //How well enemy sees in the dark
    float roamDistance;
    Counter timeSincePlayerSeen;
    Counter roamDecisionCounter;
    sf::Vector2f currentRoamPoint;    
    Counter stunCounter;
    sf::Vector2f eyeLevel;
    Direction facingDir;
    sf::Vector2f lastKnownPlayerPos;

    MovementComp movement;
    ColliderComp collider;

    AIComp(sf::Vector2f pos, sf::Vector2f size);
    virtual ~AIComp() {};
    void baseUpdate(float dt);
    void notify(sf::Vector2f pos);

    virtual void updateIdle(float dt, SpriteComp* sprite = nullptr) = 0;
    virtual void updateChasing(float dt, SpriteComp* sprite = nullptr) = 0;
    virtual void updateReturn(float dt, SpriteComp* sprite = nullptr) = 0;
    virtual void updateStunned(float dt, SpriteComp* sprite = nullptr) = 0;

    void moveLeft(SpriteComp* sprite);
    void moveRight(SpriteComp* sprite);
    void faceLeft(SpriteComp* sprite);
    void faceRight(SpriteComp* sprite);

    void setState(State state);
    State getState() const { return state; };
    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }
    void setStartPoint(sf::Vector2f point) { this->startPoint = point; }
protected:
    Direction forcedDirection;
    State state;
    sf::Vector2f startPoint;

private:
    bool decisionTime;
};