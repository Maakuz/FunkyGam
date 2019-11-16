#pragma once
#include "MovingEntity.h"
#include "Game/Misc/Counter.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Item/Item.h"
#include "Game/Item/GatherItem.h"
#include "Game/Components/SpellComp.h"
#include "Game/Components/HealthComp.h"
#include <istream>

class Player : public MovingEntity 
{
public:

    Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    friend std::istream& operator>>(std::istream& in, Player& player);

    void update(float dt, sf::Vector2f mousePos);
    void reset(sf::Vector2f spawnPoint, bool fillHealth = true);

    void setIllumination(float illumination) { this->illumination = illumination; };
    float getIllumination() const { return illumination; };
    bool isAlive() const { return this->healthComp.isAlive(); };
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

    float throwingPower;

    SpellComp spellComp;
    HealthComp healthComp;

    bool debugMode;
    bool noClip;
    bool returning;
    bool canReturn;

    int exitReached;

    void updateMove(float dt);
    void debugMove(float dt);
    void updateItems(float dt, sf::Vector2f mousePos);

};