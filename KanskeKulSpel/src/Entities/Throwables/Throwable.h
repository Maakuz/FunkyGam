#pragma once
#include "Entities/Entity.h"
#include "Entities/Item.h"
#include <fstream>

class Throwable : public Entity, public Item
{
public:
    Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture);
    ~Throwable() { };

    friend std::istream& operator>>(std::istream& in, Throwable& throwable);

    void update(float dt);

    bool hasDetonated() const { return detonated; };

    virtual void handleCollision(const Entity& collider);
private:

    float mass;
    sf::Vector2f acceleration;
    sf::Vector2f momentum;


    float armingTime;
    float armingCounter;
    float bounce;
    float collisionDelayTimer;
    bool impacted;
    bool armed;
    bool detonateOnImpact;
    bool detonated;

    int particleEffectID;
};