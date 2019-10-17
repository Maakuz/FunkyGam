#pragma once
#include "Entities/Entity.h"
#include "Entities/Item.h"
#include "Collision/Explosion.h"
#include <fstream>

class Throwable : public Entity, public Item
{
public:
    Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture);
    ~Throwable() { };

    friend std::istream& operator>>(std::istream& in, Throwable& throwable);

    void update(float dt);

    bool hasDetonated() const { return detonated; };
    void throwItem(sf::Vector2f pos, sf::Vector2f momentum);

    Explosion getExplosion() const { return explosionData; };

    virtual void handleCollision(const Entity& collider);
    virtual void handleExplosion(const Explosion& explosion) {};
private:

    float mass;
    sf::Vector2f acceleration;
    sf::Vector2f momentum;
    Explosion explosionData;


    float armingTime;
    float armingCounter;
    float bounce;
    float collisionDelayTimer;
    bool impacted;
    bool armed;
    bool detonateOnImpact;
    bool detonated;
    int damage;

    int particleEffectID;
};