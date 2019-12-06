#pragma once
#include "Misc/LuaScript.h"
#include "Game/Entities/Enemies/Bosses/Boss.h"
#include "Game/Components/AnimatedSpriteComp.h"
#include "Game/Components/SpellComp.h"
#include "Game/Components/AI/GroundAIComp.h"
#include "Game/Entities/Chain.h"
#include "Game/Entities/Hazard.h"
#include "Game/Particles/Emitter.h"

class FishMonger : public Boss
{
public:
    FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
    virtual ~FishMonger();

    virtual void spawn(sf::Vector2f pos);

    virtual void update(float dt, sf::Vector2f playerPos);

    virtual void queueHitboxes();
    virtual void handleCollision(const Collidable* collidable);
    virtual void handleExplosion(const Explosion& explosion);
private:
    struct Arm
    {
        Chain arm;
        Hazard hand;
        Arm(const sf::Texture* texture, sf::Vector2f pos, sf::Vector2f handSize, int handDamage, int linkCount = 10, int stiffness = 10):
            arm(texture, pos, linkCount, stiffness),
            hand(pos, handSize, handDamage, DamageComp::DamageOrigin::enemies){ }
    };
    
    LuaScript m_ai;
    SpellComp m_spell;

    sf::Vector2f m_armAnchor;
    sf::Vector2f m_forhead;
    sf::Vector2f m_knockback;
    Arm m_leftArm;
    Arm m_rightArm;
    Chain m_lightRope;
    std::vector<Arm> m_tentacles;
    Emitter* m_lightEmitter;

    int m_transitionEmitterID;
    Emitter* m_transitionEmitter;
    int m_spellID;

    bool m_rightSlap;
    bool m_phaseTwoInitialized;
    bool m_phaseTwo;

    void swingArms(sf::Vector2f target);

    void constrictNose();
    void initializePhaseTwo();
    void updatePhaseTwo(float dt, sf::Vector2f target);
    void updateKnockback(sf::Vector2f target);

    virtual std::istream& readSpecific(std::istream& in);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};