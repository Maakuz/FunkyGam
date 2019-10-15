#pragma once
#include "Entities/Entity.h"

class Throwable : public Entity
{
public:
    Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture);
    ~Throwable() { };

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
};