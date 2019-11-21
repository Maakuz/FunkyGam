#include "FishMonger.h"
#include "Game/Handlers/TextureHandler.h"
#include "SFML/Window/Keyboard.hpp"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Boss(data, pos, size, offset),
    leftArm(TextureHandler::get().getTexture(26), pos, 10, 5),
    rightArm(TextureHandler::get().getTexture(26), pos, 10, 5),
    leftHand(TextureHandler::get().getTexture(27), pos),
    rightHand(TextureHandler::get().getTexture(27), pos),
    ai(SCRIPT_PATH "FishmongerAI.skrop")
{
    leftArm.setMass(0.01);
    rightArm.setMass(0.01);
    rightSlap = false;
}

void FishMonger::update(float dt, sf::Vector2f playerPos)
{
    sf::Vector2f offset = collider.getSize() / 2.f;
    offset.y -= 30;
    armAnchor = movement.transform.pos + offset;

    leftArm.setPos(armAnchor, 0);
    rightArm.setPos(armAnchor, 0);

    leftArm.update(dt);
    rightArm.update(dt);
    leftHand.setPosition(leftArm.back().pos);
    rightHand.setPosition(rightArm.back().pos);

    //temp
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
        ai.reload();

    ai.runFunc("updateAI", "fffffff",
        movement.transform.pos.x, movement.transform.pos.y, 
        movement.momentum.x, movement.momentum.y, 
        playerPos.x, playerPos.y,
        dt);

    movement.acceleration.x = ai.get<float>("accel.x");
    movement.acceleration.y = ai.get<float>("accel.y");
    movement.momentum.x = ai.get<float>("momentum.x");
    movement.momentum.y = ai.get<float>("momentum.y");

    if (ai.get<bool>("swingArms"))
    {
        swingArms(playerPos - armAnchor);
    }

    if (movement.acceleration.x > 0)
    {
        if (this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    else
    {
        if (!this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    Boss::update(dt, playerPos);
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
}

void FishMonger::handleExplosion(const Explosion& explosion)
{
}

void FishMonger::swingArms(sf::Vector2f momentum)
{
    float speed = length(momentum);
    normalize(momentum);

    if (rightSlap)
        rightArm.back().pos += momentum * std::min(speed, ai.get<float>("swingSpeed"));

    else
        leftArm.back().pos += momentum * std::min(speed, ai.get<float>("swingSpeed"));


    rightSlap = !rightSlap;
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}

void FishMonger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rightArm, states);
    target.draw(rightHand, states);
    Boss::draw(target, states);
    target.draw(leftArm, states);
    target.draw(leftHand, states);
    target.draw(leftHand, states);
}
