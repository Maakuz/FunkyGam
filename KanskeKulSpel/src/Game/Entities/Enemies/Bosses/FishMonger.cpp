#include "FishMonger.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Item/Projectile/Throwables/Throwable.h"
#include "Game/Item/Projectile/Spell/Fireball.h"
#include "SFML/Window/Keyboard.hpp"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Boss(data, pos, size, offset),
    leftArm(TextureHandler::get().getTexture(26), pos, sf::Vector2f(24, 24), 30, 10, 5),
    rightArm(TextureHandler::get().getTexture(26), pos, sf::Vector2f(24, 24), 30, 10, 5),
    lightRope(TextureHandler::get().getTexture(30), pos, 10, 10),
    ai(SCRIPT_PATH "FishmongerAI.skrop")
{
    rightSlap = false;
    this->phaseTwoInitialized = false;
    this->phaseTwo = false;
    this->light = nullptr;
    this->transitionEmitterID = 0;
    this->spellID = 0;

    sf::Texture* tex = TextureHandler::get().getTexture(27);

    leftArm.arm.setTexture(tex, leftArm.arm.getLinkCount() -1);
    rightArm.arm.setTexture(tex, rightArm.arm.getLinkCount() - 1);
    lightRope.setTexture(TextureHandler::get().getTexture(29), lightRope.getLinkCount() -1);

    leftArm.arm.setMass(0.01);
    rightArm.arm.setMass(0.01);
    lightRope.setMass(0.005);

    leftArm.arm.setLinkOffset(sf::Vector2f(-1, -3), leftArm.arm.getLinkCount() - 1);
    rightArm.arm.setLinkOffset(sf::Vector2f(-1, -3), rightArm.arm.getLinkCount() - 1);
    lightRope.setLinkOffset(sf::Vector2f(0, -2));
    lightRope.setSeamless(false);
}

FishMonger::~FishMonger()
{
    if (light)
        light->kill();
}

void FishMonger::spawn(sf::Vector2f pos)
{
    light = ParticleHandler::addEmitter(13, pos);

    this->movement.transform.pos = pos;
}

void FishMonger::update(float dt, sf::Vector2f playerPos)
{
    sf::Vector2f offset = collider.getSize() / 2.f;
    offset.y -= 30;
    armAnchor = movement.transform.pos + offset;

    
    forhead = collider.getCenterPos() + sf::Vector2f(ai.get<float>("offsetX") * sprite.getTextureRect().width, ai.get<float>("offsetY"));

    leftArm.arm.setPos(armAnchor, 0);
    rightArm.arm.setPos(armAnchor, 0);

    lightRope.setPos(forhead, 0);
    constrictNose();

    leftArm.arm.update(dt);
    rightArm.arm.update(dt);
    lightRope.update(dt);
    leftArm.hand.setPos(leftArm.arm[leftArm.arm.getLinkCount() - 2].pos - leftArm.hand.getCollider().getSize() / 2.f);
    rightArm.hand.setPos(rightArm.arm[rightArm.arm.getLinkCount() - 2].pos - rightArm.hand.getCollider().getSize() / 2.f);

    //temp
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
        ai.reload();

    ai.runFunc("setHealth", "dd", health.getHealth(), health.getMaxHealth());

    ai.runFunc("updateAI", "fffffff",
        collider.getCenterPos().x, collider.getCenterPos().y, 
        movement.momentum.x, movement.momentum.y, 
        playerPos.x, playerPos.y,
        dt);
    
    movement.acceleration.x = ai.get<float>("accel.x");
    movement.acceleration.y = ai.get<float>("accel.y");
    movement.momentum.x = ai.get<float>("momentum.x");
    movement.momentum.y = ai.get<float>("momentum.y");
    this->phaseTwo = ai.get<bool>("phaseTwo");

    sprite.setAnimation(ai.get<int>("anim"));

    if (ai.get<bool>("attack"))
    {
        if (phaseTwo)
        {
            spell.startChannelling(spellID, 17);
        }

        else
            swingArms(playerPos);
    }

    if (playerPos.x > movement.transform.pos.x)
    {
        if (this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    else
    {
        if (!this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    if (!phaseTwoInitialized && ai.get<bool>("transitioning"))
        initializePhaseTwo();

    if (this->phaseTwo)
        updatePhaseTwo(dt, playerPos);


    Boss::update(dt, playerPos);
}


void FishMonger::swingArms(sf::Vector2f target)
{
    Chain* arm;
    if (rightSlap)
        arm = &this->rightArm.arm;

    else
        arm = &this->leftArm.arm;
    sf::Vector2f momentum = target - arm->back().pos;
    float speed = length(momentum);
    normalize(momentum);


    arm->back().pos += momentum * std::min(speed, ai.get<float>("swingSpeed"));


    rightSlap = !rightSlap;
}

void FishMonger::constrictNose()
{
    if (lightRope.back().pos.y > lightRope[0].pos.y)
        lightRope.back().pos.y = lightRope[0].pos.y;

    if (!this->sprite.isFlippedHorizontally())
        lightRope.back().pos.x += 0.1;

    else
        lightRope.back().pos.x -= 0.1;

    this->light->setEmitterPos(lightRope.back().pos);
}

void FishMonger::initializePhaseTwo()
{
    this->phaseTwoInitialized = true;
    sf::Vector2f displacement(1, 0);
    this->movement.mass = 0;
    this->movement.setAirRes(0.99);

    Arm tentacle(TextureHandler::get().getTexture(26), armAnchor, sf::Vector2f(24, 24), 30, 20, 5);
    tentacle.arm.setTexture(TextureHandler::get().getTexture(27), tentacle.arm.getLinkCount() - 1);
    tentacle.arm.setLinkOffset(sf::Vector2f(-1, -3), tentacle.arm.getLinkCount() - 1);
    tentacle.arm.setMass(0);

    for (int i = 0; i < 10; i++)
    {
        displacement = rotateBy(rand() % 360, displacement);

        this->tentacles.push_back(tentacle);
        this->tentacles.back().arm.back().pos += (displacement * 100.f);
    }
}

void FishMonger::updatePhaseTwo(float dt, sf::Vector2f target)
{
    this->movement.grounded = false;
    this->spell.update(dt, this->lightRope.back().pos);

    if (spell.isChannelling())
    {
        float channelTime = ai.get<float>("chanellTime");

        if (channelTime < spell.getChannelTime())
        {
            spell.castSpell<LightProjectile>(target, this);
            ai.runFunc("setChannelling", "b", false);
        }
    }

    for (Arm& arm : tentacles)
    {
        arm.arm.setPos(armAnchor, 0);
        arm.arm.update(dt);
        arm.hand.setPos(arm.arm[arm.arm.getLinkCount() - 2].pos - arm.hand.getCollider().getSize() / 2.f);
    }

    if (ai.get<bool>("swingTentacles"))
    {
        int currentTentacle = ai.get<int>("currentTentacle");

        sf::Vector2f displacement(0, 1);
        displacement = rotateBy(rand() % 360, displacement);

        tentacles[currentTentacle].arm.back().pos += displacement * ai.get<float>("swingSpeed");
    }
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}

void FishMonger::handleCollision(const Collidable* collidable)
{
    if (collidable->getCollider().hasComponent(ColliderKeys::ground) || collidable->getCollider().hasComponent(ColliderKeys::platform))
    {
        //walking on ground
        if (this->movement.momentum.y > 0 && collidable->getCollider().intersects(collidable->getCollider().getUpBox(), this->collider.getDownBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.transform.pos.y = collidable->getCollider().up() - this->collider.height();
            movement.grounded = true;
        }

        //smackin into roof
        if (collidable->getCollider().intersects(collidable->getCollider().getDownBox(), this->collider.getUpBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.transform.pos.y = collidable->getCollider().down();
        }

        if (collidable->getCollider().intersects(collidable->getCollider().getLeftBox(), this->collider.getRightBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.transform.pos.x = collidable->getCollider().left() - this->collider.width();
            this->movement.jump();
        }

        if (collidable->getCollider().intersects(collidable->getCollider().getRightBox(), this->collider.getLeftBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.transform.pos.x = collidable->getCollider().right();
            this->movement.jump();
        }
    }

    else if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collidable);
        this->health.takeDamage(throwable->getDamage());
    }

    else if (collidable->getCollider().hasComponent(ColliderKeys::fireball))
        this->health.takeDamage(dynamic_cast<const Fireball*>(collidable)->getDamage());
}

void FishMonger::handleExplosion(const Explosion& explosion)
{
    health.takeDamage(explosion.calculateDamage(collider.getCenterPos()));
}


void FishMonger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rightArm.arm, states);
    target.draw(rightArm.hand.getCollider(), states);
    Boss::draw(target, states);
    target.draw(leftArm.arm, states);
    target.draw(leftArm.hand.getCollider(), states);
    target.draw(lightRope, states);

    if (phaseTwo)
    {
        for (const Arm& arm : tentacles)
        {
            target.draw(arm.arm, states);
            target.draw(arm.hand.getCollider(), states);
        }
    }
}
