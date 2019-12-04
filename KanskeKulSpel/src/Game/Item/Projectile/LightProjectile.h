#pragma once
#include "Game/Entities/Collidable.h"
#include "Game/Components/MovementComp.h"
#include "Game/Components/DamageComp.h"
#include "Game/Particles/Emitter.h"


class LightProjectile : public Collidable
{
public:
    LightProjectile();
    virtual ~LightProjectile();
    
    friend std::istream& operator>>(std::istream& in, LightProjectile& projectile);
    friend std::ostream& operator<<(std::ostream& out, const LightProjectile& projectile);

    void update(float dt);
    void shoot(sf::Vector2f pos, sf::Vector2f dir, DamageComp::DamageOrigin origin);
    bool isDestroyed() const { return destroyed; };

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion) {};

    void setVelocity(float v) { this->velocity = v; };
    float getVelocity() const { return velocity; };

    void setInitialEmitterID(int id) { this->initialEmitterID = id; };
    int getInitialEmitterID() const { return initialEmitterID; };

    void setLightEmitterID(int id) { this->lightEmitterID = id; };
    int getLightEmitterID() const { return lightEmitterID; };

    void setImpactEmitterID(int id) { this->impactEmitterID = id; };
    int getImpactEmitterID() const { return impactEmitterID; };

    void setSize(sf::Vector2f size) { this->getComponent<ColliderComp>()->setSize(size); };
private:
    Emitter* light;
    int lightEmitterID;
    int initialEmitterID;
    int impactEmitterID;
    float velocity;
    float lifeTime;
    bool destroyed;
};