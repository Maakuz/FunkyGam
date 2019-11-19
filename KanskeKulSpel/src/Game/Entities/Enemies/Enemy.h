#pragma once
#include "Game/Misc/Counter.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Components/HealthComp.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/MovementComp.h"
#include "Game/Components/AI/AIComp.h"
#include "Game/Entities/Collidable.h"
#include <fstream>


class Enemy : public Collidable, public sf::Drawable
{
public:
    Enemy(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    ~Enemy() {};

    friend std::istream& operator>>(std::istream& in, Enemy& enemy);

    virtual void update(float dt) = 0;
    void notifyEnemy(sf::Vector2f playerPos);
    void spawn(sf::Vector2f pos);

    bool isAlive();
    bool isHealthChanged();
    float getHealthPercentage() const;

    AIComp::Direction getFacingDir() const { return this->getAI()->facingDir; }
    AIComp::State getState() const { return this->getAI()->getState(); }
    float getSightRadius() const { return this->getAI()->sightRadius; }
    float getVisionRatingAt(float distance) const;

    sf::Vector2f getEyePos() const;
    void setEyeLevel(sf::Vector2f eyeLevel) { this->getAI()->eyeLevel = eyeLevel; };
    sf::Vector2f getLastKnownPos() const { return this->getAI()->lastKnownPlayerPos; };

    const MovementComp& getMovementComp() const { return this->getAI()->movement; };


    virtual const ColliderComp& getCollider()const { return getAI()->collider; };
protected:
    HealthComp health;
    AnimatedSpriteComp sprite;
    sf::Sprite question;
    sf::Sprite exclamation;
    Counter drawExclamation;
    Counter drawQuestion;

    void updateEnemy(float dt);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    int prevHealth;

    virtual const AIComp* getAI() const = 0;
    virtual AIComp* getAI() = 0;

    virtual std::istream& readSpecific(std::istream& in) = 0;
};