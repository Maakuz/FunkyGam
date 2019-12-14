#pragma once
#include "Spell.h"
#include "Game/Particles/Emitter.h"
#include "Game/Entities/Collidable.h"
#include "Game/Components/DamageComp.h"

class Fireball : public Spell
{
public:
    Fireball(sf::Vector2f pos);
    virtual ~Fireball();

    virtual bool isComplete() const;
    virtual void cast(sf::Vector2f pos, sf::Vector2f dest, float channelTime);
    virtual void update(float dt);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);

    virtual std::istream& readSpecific(std::istream& in);
    virtual std::ostream& writeSpecific(std::ostream& out) const;

    int getMinCharge() const { return this->m_minCharge; };
    void setMinCharge(int minCharge) { this->m_minCharge = minCharge; };

    int getMaxCharge() const { return this->m_maxCharge; };
    void setMaxCharge(int maxCharge) { this->m_maxCharge = maxCharge; };

    int getTrailEmitterID() const { return this->m_trailEmitterID; };
    void setTrailEmitterID(int trailEmitterID) { this->m_trailEmitterID = trailEmitterID; };

    int getImpactEmitterID() const { return this->m_impactEmitterID; };
    void setImpactEmitterID(int impactEmitterID) { this->m_impactEmitterID = impactEmitterID; };

    int getFullTrailEmitterID() const { return this->m_fullTrailEmitterID; };
    void setFullTrailEmitterID(int fullTrailEmitterID) { this->m_fullTrailEmitterID = fullTrailEmitterID; };

    int getFullImpactEmitterID() const { return this->m_fullImpactEmitterID; };
    void setFullImpactEmitterID(int fullImpactEmitterID) { this->m_fullImpactEmitterID = fullImpactEmitterID; };

    float getMaxTravelDistance() const { return this->m_maxTravelDistance; };
    void setMaxTravelDistance(float maxTravelDistance) { this->m_maxTravelDistance = maxTravelDistance; };

    float getTopSpeed() const { return this->m_topSpeed; };
    void setTopSpeed(float topSpeed) { this->m_topSpeed = topSpeed; };

    Explosion* getExplosionPtr() { return &m_explosion; };

private:
    int m_minCharge;
    int m_maxCharge;
    
    float m_maxTravelDistance;
    float m_topSpeed;
    int m_trailEmitterID;
    int m_impactEmitterID;
    Explosion m_explosion;

    int m_fullTrailEmitterID;
    int m_fullImpactEmitterID;

    Emitter* m_trail;
    sf::Vector2f m_destination;
    sf::Vector2f m_direction;
    float m_distance;
    bool m_complete;
    bool m_fullCharge;

    float m_traveledDistance;
};