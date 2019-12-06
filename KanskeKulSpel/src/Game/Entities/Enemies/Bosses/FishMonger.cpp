#include "FishMonger.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Item/Projectile/Throwables/Throwable.h"
#include "Game/Item/Projectile/Spell/Fireball.h"
#include "SFML/Window/Keyboard.hpp"
#include "Game/Collision/CollisionHandler.h"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Boss(data, pos, size, offset),
    m_leftArm(TextureHandler::get().getTexture(26), pos, sf::Vector2f(24, 24), 15, 10, 5),
    m_rightArm(TextureHandler::get().getTexture(26), pos, sf::Vector2f(24, 24), 15, 10, 5),
    m_lightRope(TextureHandler::get().getTexture(30), pos, 10, 10),
    m_ai(SCRIPT_PATH "FishmongerAI.skrop")
{
    m_rightSlap = false;
    m_phaseTwoInitialized = false;
    m_phaseTwo = false;
    m_lightEmitter = nullptr;
    m_transitionEmitterID = 18;
    m_spellID = 0;

    sf::Texture* tex = TextureHandler::get().getTexture(27);

    m_leftArm.arm.setTexture(tex, m_leftArm.arm.getLinkCount() -1);
    m_rightArm.arm.setTexture(tex, m_rightArm.arm.getLinkCount() - 1);
    m_lightRope.setTexture(TextureHandler::get().getTexture(29), m_lightRope.getLinkCount() -1);

    m_leftArm.arm.setMass(0.01);
    m_rightArm.arm.setMass(0.01);
    m_lightRope.setMass(0.005);

    m_leftArm.arm.setLinkOffset(sf::Vector2f(-1, -3), m_leftArm.arm.getLinkCount() - 1);
    m_rightArm.arm.setLinkOffset(sf::Vector2f(-1, -3), m_rightArm.arm.getLinkCount() - 1);
    m_lightRope.setLinkOffset(sf::Vector2f(0, -2));
    m_lightRope.setSeamless(false);
}

FishMonger::~FishMonger()
{
    if (m_lightEmitter)
        ParticleHandler::destroyEmitter(m_lightEmitter);
}

void FishMonger::spawn(sf::Vector2f pos)
{
    m_lightEmitter = ParticleHandler::createEmitter(13, pos);

    getComponent<MovementComp>()->transform.pos = pos;
}

void FishMonger::update(float dt, sf::Vector2f playerPos)
{
    ColliderComp* collider = getComponent<ColliderComp>();
    MovementComp* movement = getComponent<MovementComp>();
    AnimatedSpriteComp* sprite = getComponent<AnimatedSpriteComp>();

    sf::Vector2f offset =  collider->getSize() / 2.f;
    offset.y -= 30;
    m_armAnchor = movement->transform.pos + offset;

    
    m_forhead = collider->getCenterPos() + sf::Vector2f(m_ai.get<float>("offsetX") * sprite->getTextureRect().width, m_ai.get<float>("offsetY"));

    m_leftArm.arm.setPos(m_armAnchor, 0);
    m_rightArm.arm.setPos(m_armAnchor, 0);

    m_lightRope.setPos(m_forhead, 0);
    constrictNose();

    m_leftArm.arm.update(dt);
    m_rightArm.arm.update(dt);
    m_lightRope.update(dt);
    m_leftArm.hand.setPos(m_leftArm.arm[m_leftArm.arm.getLinkCount() - 2].pos - m_leftArm.hand.getColliderComp()->getSize() / 2.f);
    m_rightArm.hand.setPos(m_rightArm.arm[m_rightArm.arm.getLinkCount() - 2].pos - m_rightArm.hand.getColliderComp()->getSize() / 2.f);

    //temp
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
        m_ai.reload();

    m_ai.runFunc("setHealth", "dd", getComponent<HealthComp>()->getCurrentHealth(), getComponent<HealthComp>()->getMaxHealth());

    m_ai.runFunc("updateAI", "fffffff",
        collider->getCenterPos().x, collider->getCenterPos().y, 
        movement->momentum.x, movement->momentum.y, 
        playerPos.x, playerPos.y,
        dt);
    
    movement->acceleration.x = m_ai.get<float>("accel.x");
    movement->acceleration.y = m_ai.get<float>("accel.y");
    movement->momentum.x = m_ai.get<float>("momentum.x");
    movement->momentum.y = m_ai.get<float>("momentum.y");
    m_phaseTwo = m_ai.get<bool>("phaseTwo");

    sprite->setAnimation(m_ai.get<int>("anim"));

    if (m_ai.get<bool>("attack"))
    {
        if (m_phaseTwo)
        {
            m_spell.startChannelling(m_spellID, 17);
        }

        else
            swingArms(playerPos);
    }

    if (playerPos.x > movement->transform.pos.x)
    {
        if (sprite->isFlippedHorizontally())
            sprite->flipHorizontally();
    }

    else
    {
        if (!sprite->isFlippedHorizontally())
            sprite->flipHorizontally();
    }

    if (!m_phaseTwoInitialized && m_ai.get<bool>("transitioning"))
        initializePhaseTwo();

    else if (m_phaseTwoInitialized && !m_phaseTwo)
        m_transitionEmitter->setEmitterPos(collider->getCenterPos());

    else if (m_phaseTwo)
    {
        updatePhaseTwo(dt, playerPos);

        if (m_transitionEmitter)
            ParticleHandler::destroyEmitter(m_transitionEmitter);
    }

    m_lightEmitter->setEmitterPos(m_lightRope.back().pos);
    ParticleHandler::queueEmitter(m_lightEmitter);

    Boss::update(dt, playerPos);
    updateKnockback(playerPos);
}


void FishMonger::swingArms(sf::Vector2f target)
{
    Chain* arm;
    if (m_rightSlap)
        arm = &m_rightArm.arm;

    else
        arm = &m_leftArm.arm;
    sf::Vector2f momentum = target - arm->back().pos;
    float speed = length(momentum);
    normalize(momentum);


    arm->back().pos += momentum * std::min(speed, m_ai.get<float>("swingSpeed"));


    m_rightSlap = !m_rightSlap;
}

void FishMonger::constrictNose()
{
    if (m_lightRope.back().pos.y > m_lightRope[0].pos.y)
        m_lightRope.back().pos.y = m_lightRope[0].pos.y;

    if (!getComponent<AnimatedSpriteComp>()->isFlippedHorizontally())
        m_lightRope.back().pos.x += 0.1;

    else
        m_lightRope.back().pos.x -= 0.1;

}

void FishMonger::initializePhaseTwo()
{
    m_phaseTwoInitialized = true;
    sf::Vector2f displacement(1, 0);
    getMovementComp()->mass = 0;
    getMovementComp()->setAirRes(0.99);

    Arm tentacle(TextureHandler::get().getTexture(26), m_armAnchor, sf::Vector2f(24, 24), 15, 20, 5);
    tentacle.arm.setTexture(TextureHandler::get().getTexture(27), tentacle.arm.getLinkCount() - 1);
    tentacle.arm.setLinkOffset(sf::Vector2f(-1, -3), tentacle.arm.getLinkCount() - 1);
    tentacle.arm.setMass(0);

    for (int i = 0; i < 10; i++)
    {
        displacement = rotateBy(rand() % 360, displacement);

        m_tentacles.push_back(tentacle);
        m_tentacles.back().arm.back().pos += (displacement * 100.f);
    }

    m_transitionEmitter = ParticleHandler::createEmitter(m_transitionEmitterID, getColliderComp()->getCenterPos());
}

void FishMonger::updatePhaseTwo(float dt, sf::Vector2f target)
{
    MovementComp* movement = getComponent<MovementComp>();
    
    movement->grounded = false;
    m_spell.update(dt, m_lightRope.back().pos);

    if (m_spell.isChannelling())
    {
        float channelTime = m_ai.get<float>("chanellTime");

        if (channelTime < m_spell.getChannelTime())
        {
            m_spell.castSpell<LightProjectile>(target, DamageComp::DamageOrigin::enemies);
            m_ai.runFunc("setChannelling", "b", false);
        }
    }

    for (Arm& arm : m_tentacles)
    {
        arm.arm.setPos(m_armAnchor, 0);
        arm.arm.update(dt);
        arm.hand.setPos(arm.arm[arm.arm.getLinkCount() - 2].pos - arm.hand.getColliderComp()->getSize() / 2.f);
    }

    if (m_ai.get<bool>("swingTentacles"))
    {
        int currentTentacle = m_ai.get<int>("currentTentacle");

        sf::Vector2f displacement(0, 1);
        displacement = rotateBy(rand() % 360, displacement);

        m_tentacles[currentTentacle].arm.back().pos += displacement * m_ai.get<float>("swingSpeed");
    }
}

void FishMonger::updateKnockback(sf::Vector2f target)
{
    float centerX = getColliderComp()->getCenterPos().x;
    float dir = (target.x - centerX) / abs(target.x - centerX);

    m_leftArm.hand.getDamageComp()->knockback = sf::Vector2f(m_knockback.x * dir, m_knockback.y);
    m_rightArm.hand.getDamageComp()->knockback = sf::Vector2f(m_knockback.x * dir, m_knockback.y);

    for (Arm& arm : m_tentacles)
        arm.hand.getDamageComp()->knockback = sf::Vector2f(m_knockback.x * dir, m_knockback.y);
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> m_knockback.x >> m_knockback.y;


    return in;
}

void FishMonger::queueHitboxes()
{
    CollisionHandler::queueCollider(&m_leftArm.hand);
    CollisionHandler::queueCollider(&m_rightArm.hand);

    for (Arm& arm : m_tentacles)
        CollisionHandler::queueCollider(&arm.hand);
}

void FishMonger::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getComponent<ColliderComp>();
    const ColliderComp* collider = getComponent<ColliderComp>();
    MovementComp* movement = getComponent<MovementComp>();

    if (otherCollider->hasComponent(ColliderKeys::ground) || otherCollider->hasComponent(ColliderKeys::platform))
    {
        //walking on ground
        if (movement->momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
        {
            movement->momentum.y = 0;
            movement->transform.pos.y = otherCollider->up() - collider->height();
            movement->grounded = true;
        }

        //smackin into roof
        if (ColliderComp::intersects(otherCollider->getDownBox(), collider->getUpBox()))
        {
            movement->momentum.y = 0;
            movement->transform.pos.y = otherCollider->down();
        }

        if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getRightBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->transform.pos.x = otherCollider->left() - collider->width();
            movement->jump();
        }

        if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getLeftBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->transform.pos.x = otherCollider->right();
            movement->jump();
        }
    }

    const DamageComp* damage = collidable->getComponent<DamageComp>();

    if (damage)
    {
        if (damage->origin != DamageComp::DamageOrigin::enemies)
            getComponent<HealthComp>()->takeDamage(damage->damage);
    }
}

void FishMonger::handleExplosion(const Explosion& explosion)
{
    getComponent<HealthComp>()->takeDamage(explosion.calculateDamage(getComponent<ColliderComp>()->getCenterPos()));
}


void FishMonger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_rightArm.arm, states);
    target.draw(*m_rightArm.hand.getColliderComp(), states);
    Boss::draw(target, states);
    target.draw(m_leftArm.arm, states);
    target.draw(*m_leftArm.hand.getColliderComp(), states);
    target.draw(m_lightRope, states);

    if (m_phaseTwo)
    {
        for (const Arm& arm : m_tentacles)
        {
            target.draw(arm.arm, states);
            target.draw(*arm.hand.getColliderComp(), states);
        }
    }
}
