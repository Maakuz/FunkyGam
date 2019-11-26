#pragma once
#include "Game/Entities/Collidable.h"
#include "Game/Components/MovementComp.h"
#include "Game/Particles/Emitter.h"


class LightProjectile : public Collidable
{
public:
    LightProjectile();
    virtual ~LightProjectile() {};
    
    friend std::istream& operator>> (std::istream& in, LightProjectile& projectile);
    friend std::ostream& operator<< (std::ostream& out, const LightProjectile& projectile);

    void update(float dt);
    void shoot(sf::Vector2f pos, sf::Vector2f dir, Collidable* owner);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion) {};
    virtual const ColliderComp& getCollider()const { return this->collider; };

    void setDamage(int damage) { this->damage = damage; };
    void setVelocity(float v) { this->velocity = v; };
    void setInitialEmitterID(int id) { this->initialEmiterID = id; };
    void setLightEmitterID(int id) { this->lightEmitterID = id; };
    void setSize(sf::Vector2f size) { this->collider.setSize(size); };

private:
    Collidable* owner;
    ColliderComp collider;
    MovementComp movement;
    Emitter* light;
    int lightEmitterID;
    int initialEmiterID;
    int damage;
    float velocity;
};