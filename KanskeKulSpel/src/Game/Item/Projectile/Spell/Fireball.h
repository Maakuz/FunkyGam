#pragma once
#include "Spell.h"
#include "Game/Particles/Emitter.h"
#include "Game/Entities/Collidable.h"
#include "Game/Components/DamageComp.h"

class Fireball : public Spell
{
public:
    Fireball(sf::Vector2f pos);
    virtual ~Fireball() { };

    virtual bool isComplete() const;
    virtual void cast(sf::Vector2f pos, sf::Vector2f dest, float channelTime);
    virtual void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);

    virtual std::istream& readSpecific(std::istream& in);
    virtual std::ostream& writeSpecific(std::ostream& out) const;

    int getMinCharge() const { return this->minCharge; };
    void setMinCharge(int minCharge) { this->minCharge = minCharge; };

    int getMaxCharge() const { return this->maxCharge; };
    void setMaxCharge(int maxCharge) { this->maxCharge = maxCharge; };

    int getTrailEmitterID() const { return this->trailEmitterID; };
    void setTrailEmitterID(int trailEmitterID) { this->trailEmitterID = trailEmitterID; };

    int getImpactEmitterID() const { return this->impactEmitterID; };
    void setImpactEmitterID(int impactEmitterID) { this->impactEmitterID = impactEmitterID; };

    int getFullTrailEmitterID() const { return this->fullTrailEmitterID; };
    void setFullTrailEmitterID(int fullTrailEmitterID) { this->fullTrailEmitterID = fullTrailEmitterID; };

    int getFullImpactEmitterID() const { return this->fullImpactEmitterID; };
    void setFullImpactEmitterID(int fullImpactEmitterID) { this->fullImpactEmitterID = fullImpactEmitterID; };

    float getMaxTravelDistance() const { return this->maxTravelDistance; };
    void setMaxTravelDistance(float maxTravelDistance) { this->maxTravelDistance = maxTravelDistance; };

    float getTopSpeed() const { return this->topSpeed; };
    void setTopSpeed(float topSpeed) { this->topSpeed = topSpeed; };

    Explosion* getExplosionPtr() { return &explosion; };

private:
    int minCharge;
    int maxCharge;
    
    float maxTravelDistance;
    float topSpeed;
    int trailEmitterID;
    int impactEmitterID;
    Explosion explosion;

    int fullTrailEmitterID;
    int fullImpactEmitterID;

    Emitter* trail;
    sf::Vector2f destination;
    sf::Vector2f direction;
    float distance;
    bool complete;
    bool fullCharge;

    float traveledDistance;
};