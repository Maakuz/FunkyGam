#pragma once
#include "../Entity.h"
#include "Game/Misc/Counter.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Components/HealthComp.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/MovementComp.h"
#include "Game/Entities/Collidable.h"
#include <fstream>


class Enemy : public Entity, public Collidable, public sf::Drawable
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

    Enemy(AnimationData data, sf::Vector2f pos, UIHandler* ui, sf::Vector2f size, sf::Vector2f offset);
    ~Enemy() {};

    friend std::istream& operator>>(std::istream& in, Enemy& enemy);

    virtual void update(float dt) = 0;
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

    const MovementComp& getMovementComp() const { return this->movement; };


    virtual const ColliderComp& getCollider()const { return collider; };
protected:
    UIHandler* ui;
    float roamDistance;
    State state;
    Direction facingDir;
    Direction forcedDirection;
    sf::Vector2f eyeLevel;
    Counter timeSincePlayerSeen;
    sf::Vector2f currentRoamPoint;

    HealthComp health;
    AnimatedSpriteComp sprite;
    MovementComp movement;
    ColliderComp collider;

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
    void faceLeft();
    void faceRight();

    void desicionTimeOver();
    bool isDesicionTime() const { return this->decisionTime; }

    sf::Vector2f getStartPoint() const { return this->startPoint; }
    void setStartPoint(sf::Vector2f point) { this->startPoint = point; }
    void updateEnemy(float dt);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    Counter roamDecisionCounter;
    bool decisionTime;
    int prevHealth;
    sf::Vector2f startPoint;
    sf::Vector2f lastKnownPlayerPos;

    virtual std::istream& readSpecific(std::istream& in) = 0;

};