#include "FishMonger.h"
#include "Game/Handlers/TextureHandler.h"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Boss(data, pos, size, offset),
    leftArm(TextureHandler::get().getTexture(17), pos, 10, 5),
    rightArm(TextureHandler::get().getTexture(17), pos, 10, 5),
    ai(SCRIPT_PATH "FishmongerAI.skrop")
{
    leftArm.setMass(0.1);
    rightArm.setMass(0.1);
}

void FishMonger::update(float dt)
{
    armAnchor = movement.transform.pos;

    leftArm.setPos(armAnchor, 0);
    rightArm.setPos(armAnchor, 0);

    leftArm.update(dt);
    rightArm.update(dt);

    Boss::update(dt);
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

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}

void FishMonger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rightArm, states);
    Boss::draw(target, states);
    target.draw(leftArm, states);
}
