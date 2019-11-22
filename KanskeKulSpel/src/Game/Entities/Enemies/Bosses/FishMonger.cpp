#include "FishMonger.h"
#include "Game/Handlers/TextureHandler.h"
#include "SFML/Window/Keyboard.hpp"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Boss(data, pos, size, offset),
    leftArm(TextureHandler::get().getTexture(26), pos, 10, 5),
    rightArm(TextureHandler::get().getTexture(26), pos, 10, 5),
    leftHand(pos, sf::Vector2f(24, 24), 30),
    rightHand(pos, sf::Vector2f(24, 24), 30),
    ai(SCRIPT_PATH "FishmongerAI.skrop")
{
    rightSlap = false;

    std::vector <const sf::Texture*> textures;

    for (int i = 0; i < leftArm.getLinkCount() - 1; i++)
    {
        textures.push_back(TextureHandler::get().getTexture(26));
    }

    textures.push_back(TextureHandler::get().getTexture(27));

    leftArm = Chain(textures, pos, leftArm.getLinkCount(), 5);
    rightArm = Chain(textures, pos, rightArm.getLinkCount(), 5);

    leftArm.setMass(0.01);
    rightArm.setMass(0.01);

    leftArm.setLinkOffset(sf::Vector2f(-1, -3), leftArm.getLinkCount() - 1);
    rightArm.setLinkOffset(sf::Vector2f(-1, -3), rightArm.getLinkCount() - 1);
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
    leftHand.setPos(leftArm[leftArm.getLinkCount() - 2].pos - leftHand.getCollider().getSize() / 2.f);
    rightHand.setPos(rightArm[leftArm.getLinkCount() - 2].pos - rightHand.getCollider().getSize() / 2.f);

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

    sprite.setAnimation(ai.get<int>("anim"));


    if (ai.get<bool>("swingArms"))
    {
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
    health.takeDamage(explosion.calculateDamage(collider.getCenterPos()));
}

void FishMonger::swingArms(sf::Vector2f target)
{
    Chain* arm;
    if (rightSlap)
        arm = &this->rightArm;

    else
        arm = &this->leftArm;
    sf::Vector2f momentum = target - arm->back().pos;
    float speed = length(momentum);
    normalize(momentum);


    arm->back().pos += momentum * std::min(speed, ai.get<float>("swingSpeed"));


    rightSlap = !rightSlap;
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}

void FishMonger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rightArm, states);
    target.draw(rightHand.getCollider(), states);
    Boss::draw(target, states);
    target.draw(leftArm, states);
    target.draw(leftHand.getCollider(), states);
}
