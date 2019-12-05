#pragma once
#include "Game/Misc/Counter.h"
#include "Entity.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Item/Item.h"
#include "Game/Item/GatherItem.h"
#include "Game/Entities/Collidable.h"
#include "Game/Components/MovementComp.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/TomeComp.h"
#include "Game/Components/HealthComp.h"
#include <istream>

class Player : public Collidable, public sf::Drawable
{
public:
    
    Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(0, 0));
    virtual ~Player() {};

    friend std::istream& operator>>(std::istream& in, Player& player);

    void update(float dt, sf::Vector2f mousePos);
    void reset(sf::Vector2f spawnPoint, bool fillHealth = true);

    void setIllumination(float illumination) { this->illumination = illumination; };
    float getIllumination() const { return illumination; };
    bool isAlive() const { return this->getHealthComp()->isAlive(); };
    bool isReturning() const { return this->returning; };
    int getExitReached() const { return this->exitReached; };

    void setGatherableInRange(GatherItem* item) { this->gatherableInRange = item; };

    virtual void handleCollision(const Collidable* collider);
    virtual void handleExplosion(const Explosion& explosion);
private:
    Counter platformPassingCounter;
    Counter invincibilityCounter;
    UIHandler* ui;
    GatherItem* gatherableInRange;
    float illumination;

    float throwingPower;

    bool debugMode;
    bool noClip;
    bool returning;
    bool canReturn;

    int exitReached;

    void updateMove(float dt);
    void debugMove(float dt);
    void updateItems(float dt, sf::Vector2f mousePos);
    void takeDamage(int damage);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};