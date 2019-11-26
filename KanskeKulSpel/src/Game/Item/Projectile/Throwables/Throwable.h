#pragma once
#include "Game/Item/Item.h"
#include "Game/Entities/Collidable.h"
#include "Game/Components/MovementComp.h"

class Throwable : public Item, public Collidable
{
public:
    Throwable(sf::Vector2f pos, const sf::Texture* texture, sf::Vector2f size);
    ~Throwable() { };

    void update(float dt);

    bool hasDetonated() const { return detonated; };

    void throwItem(sf::Vector2f pos, sf::Vector2f momentum, const Collidable* thrower);

    const Explosion& getExplosion() const { return explosionData; };
    Explosion* getExplosionPtr() { return &this->explosionData; };
    sf::Vector2f getMomentum() const { return this->movement.momentum; };

    int getDamage() const { return this->damage; };
    void setDamage(int damage) { this->damage = damage; };

    float getMass()const { return movement.mass; };
    void setMass(float mass) { this->movement.mass = mass; };

    int getArmingTime()const { return this->armingTime; };
    void setArmingTime(int time) { this->armingTime = time; };

    float getBounce()const { return this->bounce; };
    void setBounce(float bounce) { this->bounce = bounce; };

    bool isDetonatingOnImpact()const { return detonateOnImpact; };
    void setDetonatingOnImpact(bool detonateOnImpact) { this->detonateOnImpact = detonateOnImpact; };

    int getParticleEffectID()const { return particleEffectID; };
    void setParticleEffectID(int effectID) { this->particleEffectID = effectID; };

    //Careful with this, could crash if thrower is gone
    const Collidable* getThrower()const { return this->thrower; };

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion) {};
    virtual const ColliderComp& getCollider()const { return collider; };

private:
    Explosion explosionData;
    const Collidable* thrower;

    MovementComp movement;
    ColliderComp collider;

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
    virtual std::ostream& writeSpecific(std::ostream& out) const;
};