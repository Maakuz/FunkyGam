#pragma once
#include "Spell.h"

class Fireball : public Spell
{
public:
    Fireball(sf::Vector2f pos, const sf::Texture* texture);
    ~Fireball() { };

    virtual bool isComplete() const;
    virtual void cast();
    virtual void update(float dt);

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion);

    virtual std::istream& readSpecific(std::istream& in);
    virtual std::ostream& writeSpecific(std::ostream& out) const;

    int getDamage() const { return this->damage; };
    void setDamage(int damage) { this->damage = damage; };

    int getMinCharge() const { return this->minCharge; };
    void setMinCharge(int minCharge) { this->minCharge = minCharge; };

    int getMaxCharge() const { return this->maxCharge; };
    void setMaxCharge(int maxCharge) { this->maxCharge = maxCharge; };

    int getTrailEmitterID() const { return this->trailEmitterID; };
    void setTrailEmitterID(int trailEmitterID) { this->trailEmitterID = trailEmitterID; };

    int getImpactEmitterID() const { return this->impactEmitterID; };
    void setImpactEmitterID(int impactEmitterID) { this->impactEmitterID = impactEmitterID; };

    float getMaxTravelDistance() const { return this->maxTravelDistance; };
    void setMaxTravelDistance(float maxTravelDistance) { this->maxTravelDistance = maxTravelDistance; };

    float getRadius() const { return this->radius; };
    void setRadius(float radius) { this->radius = radius; };

private:
    int damage;
    float maxTravelDistance;
    float radius;
    int minCharge;
    int maxCharge;
    int trailEmitterID;
    int impactEmitterID;
};