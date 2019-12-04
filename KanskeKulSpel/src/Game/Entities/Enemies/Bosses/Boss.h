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

    virtual void handleCollision(const Collidable* collidable) = 0;
    virtual void handleExplosion(const Explosion& explosion) = 0;

    bool isInterrupting() { return interrupting; };
    virtual void update(float dt, sf::Vector2f playerPos);
    virtual void spawn(sf::Vector2f pos);

    bool isAlive() { return getComponent<HealthComp>()->isAlive(); };
    bool isHealthChanged();

protected:
    sf::Vector2f playerPos;

    int prevHealth;
    bool interrupting;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};