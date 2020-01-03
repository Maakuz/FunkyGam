#pragma once
#include "SFML/Graphics/Drawable.hpp"
#include "Game/Entities/Collidable.h"
#include "Game/Components/MovementComp.h"
#include "Game/Components/DamageComp.h"
#include "Game/Components/SpriteComp.h"

class Throwable : public Collidable, public sf::Drawable
{
public:
    Throwable(sf::Vector2f pos, const sf::Texture* texture);
    virtual ~Throwable() { };

    friend std::istream& operator>>(std::istream& in, Throwable& throwable);
    friend std::ostream& operator<<(std::ostream& out, const Throwable& throwable);

    void update(float dt);

    bool hasDetonated() const { return detonated; };

    void throwItem(sf::Vector2f pos, sf::Vector2f momentum, DamageComp::DamageOrigin origin);

    const Explosion& getExplosion() const { return explosionData; };
    Explosion* getExplosionPtr() { return &this->explosionData; };

    int getArmingTime()const { return this->armingTime; };
    void setArmingTime(int time) { this->armingTime = time; };

    float getBounce()const { return this->bounce; };
    void setBounce(float bounce) { this->bounce = bounce; };

    bool isDetonatingOnImpact()const { return detonateOnImpact; };
    void setDetonatingOnImpact(bool detonateOnImpact) { this->detonateOnImpact = detonateOnImpact; };

    int getParticleEffectID()const { return particleEffectID; };
    void setParticleEffectID(int effectID) { this->particleEffectID = effectID; };

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion) {};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    Explosion explosionData;

    float armingTime;
    float armingCounter;
    float bounce;
    bool impacted;
    bool armed;
    bool detonateOnImpact;
    bool detonated;

    int particleEffectID;
};