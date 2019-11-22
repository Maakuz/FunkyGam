#pragma once
#include "Misc/LuaScript.h"
#include "Game/Entities/Enemies/Bosses/Boss.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/AI/GroundAIComp.h"
#include "Game/Entities/Chain.h"
#include "Game/Entities/Hazard.h"

class FishMonger : public Boss
{
public:
    FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    virtual ~FishMonger() {};

    virtual void update(float dt, sf::Vector2f playerPos);

    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);
private:
    LuaScript ai;

    sf::Vector2f armAnchor;
    Chain leftArm;
    Chain rightArm;
    Hazard leftHand;
    Hazard rightHand;

    bool rightSlap;

    void swingArms(sf::Vector2f target);

    virtual std::istream& readSpecific(std::istream& in);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};