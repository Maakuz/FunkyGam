#include "Player.h"
#include "Game/Misc/KeyboardState.h"
#include "Game/Misc/MouseState.h"
#include "Game/Misc/VectorFunctions.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Enemies/Grunt.h"

#define TESTING false

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos, sf::Vector2f size)
    :Collidable(pos, size),
    sprite(data, pos),
    spellComp(getComponent<ColliderComp>()->getCenterPos())
{
    this->getComponent<ColliderComp>()->addComponent(ColliderKeys::player);
    this->getComponent<ColliderComp>()->addComponent(ColliderKeys::character);

    addComponent(new MovementComp);

    this->ui = uiHandler;
    this->illumination = 0;
    this->throwingPower = 0;
    this->exitReached = -1;
    this->debugMode = false;
    this->noClip = false;
    this->canReturn = false;
    this->returning = false;
    this->gatherableInRange = nullptr;

    platformPassingCounter.stopValue = 1000;
    platformPassingCounter.counter = platformPassingCounter.stopValue;

    this->sprite.spriteOffset.y = -(abs(sprite.getTextureRect().height) - size.y);
    this->sprite.spriteOffset.x = -size.x / 2.f;

    
   /* ConsoleWindow::get().addCommand("setPos", [&](Arguments args)->std::string 
        {
            if (args.size() >= 2)
            {
                try
                {
                    int x = std::stoi(args[0]);
                    int y = std::stoi(args[1]);

                     getComponent<MovementComp>()->transform.pos = (sf::Vector2f(x, y));
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid coordinates. ";
                    ret.append(e.what());
                    return ret;
                }
            }

            else
                return "missing argument int x, int y";


            return "Position set";
        });
    */
    ConsoleWindow::get().addCommand("debugMode", [&](Arguments args)->std::string
        {
            if (args.size() >= 1)
            {
                try
                {
                    int x = std::stoi(args[0]);

                    debugMode = x;
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid argument 0 or 1.";
                    return ret;
                }
            }

            else
                return "missing argument 0 or 1";


            return "The wondow is somewhere";
        });

    ConsoleWindow::get().addCommand("setHealth", [&](Arguments args)->std::string
        {
            if (args.size() >= 1)
            {
                try
                {
                    int x = std::stoi(args[0]);

                    healthComp.setHealth(x);
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid argument int.";
                    return ret;
                }
            }

            else
                return "missing argument int health";


            return "The life is liven";
        });

}

void Player::update(float dt, sf::Vector2f mousePos)
{
    MovementComp* movement = getComponent<MovementComp>();

    if (debugMode)
    {
        ImGui::Begin("Player tweaker", &debugMode);

        ImGui::DragFloat("Walk speed", &movement->walkSpeed, 0.01, 0, 10000);
        ImGui::DragFloat("Jump height", &movement->jumpHeight, 0.01, 0, 10000);
        ImGui::DragFloat("Mass", &movement->mass, 0.01, 0, 10000);

        ImGui::Checkbox("Noclip", &noClip);

        ImGui::End();
    }

    if (!noClip)
        this->updateMove(dt);

    else
        this->debugMove(dt);

    if (movement->grounded)
        sprite.setAnimation(0);

    else
        sprite.setAnimation(1);

    movement->update(dt);

    updateItems(dt, mousePos);
    spellComp.update(dt, getComponent<ColliderComp>()->getCenterPos());
    getComponent<ColliderComp>()->setPosition(movement->transform.pos);
    sprite.updateAnimation(dt);
    sprite.setPosition(movement->transform.pos);

    if (this->canReturn && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        this->returning = true;

    if (this->gatherableInRange != nullptr && KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::E))
    {
        this->gatherableInRange->item.pluck();
        this->ui->getInventory()->addItem(this->gatherableInRange->item.getID(), this->gatherableInRange->count);
    }

    ui->setHealthPercentage(this->healthComp.getHealth() / float(this->healthComp.getMaxHealth()));
    this->canReturn = false;

    if (abs(movement->momentum.x) < movement->walkSpeed * 0.75f && !sprite.isIdle())
        sprite.pauseAnimation();

    else if (abs(movement->momentum.x) > movement->walkSpeed * 0.75f)
        sprite.resumeAnimation();


}

void Player::reset(sf::Vector2f spawnPoint, bool fillHealth)
{
    MovementComp* movement = getComponent<MovementComp>();
    movement->transform.pos = spawnPoint;
    this->returning = false;
    this->exitReached = -1;
    movement->reset();

    if (fillHealth)
        this->healthComp.fillHealth();
}

void Player::updateMove(float dt)
{
    MovementComp* movement = getComponent<MovementComp>();

    movement->acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        movement->acceleration.x = 1;

        if (this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        movement->acceleration.x = -1;

        if (!this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        movement->jump();
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        platformPassingCounter.reset();

    if (movement->isJumping() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        movement->stopJump(0.3);


    platformPassingCounter.update(dt);

}

void Player::debugMove(float dt)
{
    MovementComp* movement = getComponent<MovementComp>();

    movement->acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement->acceleration.x = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement->acceleration.x = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        movement->acceleration.y = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        movement->acceleration.y = 1;
}

void Player::updateItems(float dt, sf::Vector2f mousePos)
{
    if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Button::Right))
    {
        int itemID = this->ui->getInventory()->useSelectedItem();
        if (itemID != -1)
        {
            if (dynamic_cast<const Throwable*>(ItemHandler::getTemplate(itemID)))
            {
                MovementComp* movement = getComponent<MovementComp>();

                sf::Vector2f direction = mousePos - movement->transform.pos - sf::Vector2f(16, 10);
                normalize(direction);
                direction *= (this->throwingPower);

                direction += movement->momentum * 0.5f;

                ProjectileHandler::addThrowable(itemID, movement->transform.pos, direction, this);
            }

            else if (dynamic_cast<const Tome*>(ItemHandler::getTemplate(itemID)))
            {
                spellComp.startChannelling(itemID);
            }
        }
    }

    if (spellComp.isChannelling()  && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        spellComp.castSpell(mousePos);
    }

    for (int i = 0; i < 5; i++)
    {
        if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(27 + i))) //keys 1 to 5
        {
            ui->getInventory()->setSelectedItem(i);
            spellComp.stopChannelling();
        }
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::I))
    {
        if (ui->getInventory()->isInventoryOpen())
            ui->getInventory()->closeInventory();

        else
            ui->getInventory()->openInventory();
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}

void Player::handleCollision(const Collidable* colliderEntity)
{
    ColliderComp* collider = getComponent<ColliderComp>();
    const ColliderComp* otherCollider = colliderEntity->getComponent<ColliderComp>();
    if (!noClip)
    {
        MovementComp* movement = getComponent<MovementComp>();

        if (movement->momentum.y > 0 && otherCollider->hasComponent(ColliderKeys::platform))
        {
            //walking on ground
            if (ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
            {
                if (platformPassingCounter.isTimeUp())
                {
                    movement->momentum.y = 0;
                    movement->transform.pos.y = (otherCollider->up() - collider->height());
                    movement->grounded = true;
                }
            }
        }

        else if (otherCollider->hasComponent(ColliderKeys::ground))
        {
            //walking on ground
            if (movement->momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
            {
                movement->momentum.y = 0;
                movement->transform.pos.y = (otherCollider->up() - collider->height());
                movement->grounded = true;
            }

            //smackin into roof
            if (movement->momentum.y < 0 && ColliderComp::intersects(otherCollider->getDownBox(), collider->getUpBox()))
            {
                movement->momentum.y = 0;
                movement->transform.pos.y = (otherCollider->down());
            }

            if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getRightBox()))
            {
                movement->momentum.x *= -0.5f;
                movement->transform.pos.x = (otherCollider->left() - collider->width());
            }

            if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getLeftBox()))
            {
                movement->momentum.x *= -0.5f;
                movement->transform.pos.x = (otherCollider->right());
            }
        }

        else if (otherCollider->hasComponent(ColliderKeys::grunt))
        {
            const Grunt* ptr = dynamic_cast<const Grunt*>(colliderEntity);
            if (ptr->isAttacking())
            {
                movement->addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider->getCenterPos(), otherCollider->getCenterPos(), movement->momentum, ptr->getMovementComp().momentum, movement->mass, ptr->getMovementComp().mass));
                healthComp.takeDamage(ptr->getDamage());
            }
        }

        else if (otherCollider->hasComponent(ColliderKeys::projectile))
        {
            const LightProjectile* ptr = dynamic_cast<const LightProjectile*>(colliderEntity);
            healthComp.takeDamage(ptr->getDamage());
        }
    }

    if (otherCollider->hasComponent(ColliderKeys::levelReturn))
        this->returning = true;

    else if (otherCollider->hasComponent(ColliderKeys::levelWarp))
        this->canReturn = true;

    else if (otherCollider->hasComponent(ColliderKeys::customTerrain))
    {
        std::string flag = otherCollider->getFlag();
        if (flag.compare(0, 4, "exit") == 0)
            this->exitReached = flag[4] - '0';
    }

    else if (otherCollider->hasComponent(ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(colliderEntity);
        if (throwable->getThrower() != this)
            this->healthComp.takeDamage(throwable->getDamage());
    }
}

void Player::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(getComponent<ColliderComp>()->getCenterPos());
        this->healthComp.takeDamage(damage);
    }
}

std::istream& operator>>(std::istream& in, Player& player)
{
    std::string trash;
    int health = 0;
    MovementComp* movement = player.getComponent<MovementComp>();
    in >> trash;

    in >> trash >> movement->walkSpeed;
    in >> trash >> movement->jumpHeight;
    in >> trash >> movement->mass;
    in >> trash >> health;
    in >> trash >> player.throwingPower;

    player.healthComp.setMaxHealth(health);
    player.healthComp.setHealth(health);

    return in;
}
