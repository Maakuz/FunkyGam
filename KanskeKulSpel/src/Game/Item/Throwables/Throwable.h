#pragma once
#include "Game/Item/Item.h"
#include "Game/Collision/Explosion.h"

class Throwable : public Item
{
public:
    Throwable(sf::Vector2f momentum, sf::Vector2f pos, const sf::Texture* texture);
    ~Throwable() { };

    void update(float dt);

    bool hasDetonated() const { return detonated; };

    void throwItem(sf::Vector2f pos, sf::Vector2f momentum, const Entity* thrower);

    const Explosion& getExplosion() const { return explosionData; };
    Explosion* getExplosionPtr() { return &this->explosionData; };
    sf::Vector2f getMomentum() const { return this->momentum; };
    int getDamage() const { return this->damage; };
    void setDamage(int damage) { this->damage = damage; };

    float getMass()const { return mass; };
    void setMass(float mass) { this->mass = mass; };

    int getArmingTime()const { return this->armingTime; };
    void setArmingTime(int time) { this->armingTime = time; };

    float getBounce()const { return this->bounce; };
    void setBounce(float bounce) { this->bounce = bounce; };

    bool isDetonatingOnImpact()const { return detonateOnImpact; };
    void setDetonatingOnImpact(bool detonateOnImpact) { this->detonateOnImpact = detonateOnImpact; };

    int getParticleEffectID()const { return particleEffectID; };
    void setParticleEffectID(int effectID) { this->particleEffectID = effectID; };


    virtual void handleCollision(const Entity* colliderObj);
    virtual void handleExplosion(const Explosion& explosion) {};
private:

    float mass;
    sf::Vector2f acceleration;
    sf::Vector2f momentum;
    Explosion explosionData;
    const Entity* thrower;

    bool addedMomentum;
    sf::Vector2f collisionMomentum;

    float armingTime;
    float armingCounter;
    float bounce;
    bool impacted;
    bool armed;
    bool detonateOnImpact;
    bool detonated;
    int damage;

    int particleEffectID;
    
    void addCollisionMomentum(sf::Vector2f colliderMomentum, sf::Vector2f colliderPos, float colliderMass);

    virtual std::istream& readSpecific(std::istream& in);
    virtual std::ostream& writeSpecific(std::ostream& out) const;
};