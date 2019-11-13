#pragma once
#include "MovingEntity.h"
#include "Game/Misc/Counter.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Item/Item.h"
#include "Game/Item/GatherItem.h"
#include <istream>

class Player : public MovingEntity 
{
public:

    Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    friend std::istream& operator>>(std::istream& in, Player& player);

    void update(float dt, sf::Vector2f mousePos);
    void reset(sf::Vector2f spawnPoint);

    void setIllumination(float illumination) { this->illumination = illumination; };
    float getIllumination() const { return illumination; };
    bool isAlive() const { return this->health > 0; };
    bool isReturning() const { return this->returning; };
    int getExitReached() const { return this->exitReached; };

    void setGatherableInRange(GatherItem* item) { this->gatherableInRange = item; };

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion);

private:
    Counter platformPassingCounter;
    UIHandler* ui;
    GatherItem* gatherableInRange;
    float illumination;
    int health;
    int maxHealth;
    float throwingPower;

    bool debugMode;
    bool noClip;
    bool returning;
    bool canReturn;
    int exitReached;

    void move(float dt);
    void debugMove(float dt);
    void useItem(sf::Vector2f mousePos);

};