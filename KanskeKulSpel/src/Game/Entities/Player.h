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

    void setIllumination(float illumination) { this->m_illumination = illumination; };
    float getIllumination() const { return m_illumination; };
    bool isAlive() const { return this->getHealthComp()->isAlive(); };
    bool isReturning() const { return this->m_returning; };
    int getExitReached() const { return this->m_exitReached; };

    void setGatherableInRange(GatherItem* item) { this->m_gatherableInRange = item; };

    virtual void handleCollision(const Collidable* collider);
    virtual void handleExplosion(const Explosion& explosion);
private:
    Counter m_platformPassingCounter;
    Counter m_invincibilityCounter;
    UIHandler* m_ui;
    GatherItem* m_gatherableInRange;
    float m_illumination;

    float m_throwingPower;

    bool m_debugMode;
    bool m_noClip;
    bool m_returning;
    bool m_canReturn;

    int m_exitReached;

    void updateMove(float dt);
    void debugMove(float dt);
    void updateItems(float dt, sf::Vector2f mousePos);
    void takeDamage(int damage);

    void handleGround(const ColliderComp* otherCollider);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};