#pragma once
#include "Game/Components/HealthComp.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/MovementComp.h"
#include "Game/Entities/Collidable.h"

class Boss : public Collidable, public sf::Drawable
{
public:
    Boss(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    virtual ~Boss() {};

    friend std::istream& operator>>(std::istream& in, Boss& boss);

    virtual void queueHitboxes() = 0;
    virtual void handleCollision(const Collidable* collidable) = 0;
    virtual void handleExplosion(const Explosion& explosion) = 0;

    bool isInterrupting() { return m_interrupting; };
    virtual void update(float dt, sf::Vector2f playerPos);
    virtual void spawn(sf::Vector2f pos);

    bool isAlive() { return getHealthComp()->isAlive(); };
    bool isHealthChanged();

protected:
    int m_prevHealth;
    bool m_interrupting;

    virtual std::istream& readSpecific(std::istream& in) = 0;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};