#pragma once
#include "Entities/Items/Item.h"
#include "Collision/Explosion.h"

class Throwable : public Item
{
public:
    Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture);
    ~Throwable() { };

    void update(float dt);

    bool hasDetonated() const { return detonated; };

    void throwItem(sf::Vector2f pos, sf::Vector2f momentum, const Entity* thrower);

    Explosion getExplosion() const { return explosionData; };
    int getDamage() const { return this->damage; };
    sf::Vector2f getMomentum() const { return this->momentum; };

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion) {};
private:

    float mass;
    sf::Vector2f acceleration;
    sf::Vector2f momentum;
    Explosion explosionData;
    const Entity* thrower;

    float armingTime;
    float armingCounter;
    float bounce;
    bool impacted;
    bool armed;
    bool detonateOnImpact;
    bool detonated;
    int damage;

    int particleEffectID;

    virtual std::istream& readSpecific(std::istream& in);
};