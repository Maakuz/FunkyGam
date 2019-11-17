#include "Player.h"
#include "Game/Misc/KeyboardState.h"
#include "Game/Misc/MouseState.h"
#include "Game/Misc/VectorFunctions.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Item/ProjectileHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Enemies/Grunt.h"

#define TESTING false

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos, sf::Vector2f size)
    :Entity(pos),
    sprite(data, pos),
    collider(size, pos),
    spellComp(collider.getCenterPos())
{
    this->collider.addComponent(ColliderKeys::player);
    this->collider.addComponent(ColliderKeys::character);
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

    
    ConsoleWindow::get().addCommand("setPos", [&](Arguments args)->std::string 
        {
            if (args.size() >= 2)
            {
                try
                {
                    int x = std::stoi(args[0]);
                    int y = std::stoi(args[1]);

                    pos = (sf::Vector2f(x, y));
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
    if (debugMode)
    {
        ImGui::Begin("Player tweaker", &debugMode);

        ImGui::DragFloat("Walk speed", &this->movement.walkSpeed, 0.01, 0, 10000);
        ImGui::DragFloat("Jump height", &this->movement.jumpHeight, 0.01, 0, 10000);
        ImGui::DragFloat("Mass", &this->movement.mass, 0.01, 0, 10000);

        ImGui::Checkbox("Noclip", &noClip);

        ImGui::End();
    }

    if (!noClip)
        this->updateMove(dt);

    else
        this->debugMove(dt);

    if (this->movement.grounded)
        sprite.setAnimation(0);

    else
        sprite.setAnimation(1);

    this->pos = movement.update(dt, getPosition());

    updateItems(dt, mousePos);
    spellComp.update(dt, collider.getCenterPos());
    collider.setPosition(getPosition());
    sprite.updateAnimation(dt);
    sprite.update(getPosition());

    if (this->canReturn && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        this->returning = true;

    if (this->gatherableInRange != nullptr && KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::E))
    {
        this->gatherableInRange->item.pluck();
        this->ui->getInventory()->addItem(this->gatherableInRange->item.getID(), this->gatherableInRange->count);
    }

    ui->setHealthPercentage(this->healthComp.getHealth() / float(this->healthComp.getMaxHealth()));
    this->canReturn = false;

    if (abs(movement.momentum.x) < this->movement.walkSpeed * 0.75f && !sprite.isIdle())
        sprite.pauseAnimation();

    else if (abs(movement.momentum.x) > this->movement.walkSpeed * 0.75f)
        sprite.resumeAnimation();


}

void Player::reset(sf::Vector2f spawnPoint, bool fillHealth)
{
    this->pos = spawnPoint;
    this->returning = false;
    this->exitReached = -1;
    this->movement.reset();

    if (fillHealth)
        this->healthComp.fillHealth();
}

void Player::updateMove(float dt)
{
    this->movement.acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->movement.acceleration.x = 1;

        if (this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->movement.acceleration.x = -1;

        if (!this->sprite.isFlippedHorizontally())
            this->sprite.flipHorizontally();
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        movement.jump();
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        platformPassingCounter.reset();

    if (this->movement.isJumping() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        movement.stopJump(0.3);


    platformPassingCounter.update(dt);

}

void Player::debugMove(float dt)
{
    this->movement.acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        this->movement.acceleration.x = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        this->movement.acceleration.x = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        this->movement.acceleration.y = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        this->movement.acceleration.y = 1;
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
                sf::Vector2f direction = mousePos - this->getPosition() - sf::Vector2f(16, 10);
                normalize(direction);
                direction *= (this->throwingPower);

                direction += movement.momentum * 0.5f;

                ProjectileHandler::addThrowable(itemID, this->getPosition(), direction, this);
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
    if (!noClip)
    {
        if (movement.momentum.y > 0 && colliderEntity->getCollider().hasComponent(ColliderKeys::platform))
        {
            //walking on ground
            if (ColliderComp::intersects(colliderEntity->getCollider().getUpBox(), this->collider.getDownBox()))
            {
                if (platformPassingCounter.isTimeUp())
                {
                    this->movement.momentum.y = 0;
                    this->pos.y = (colliderEntity->getCollider().up() - collider.height());
                    this->movement.grounded = true;
                }
            }
        }

        else if (colliderEntity->getCollider().hasComponent(ColliderKeys::ground))
        {
            //walking on ground
            if (this->movement.momentum.y > 0 && colliderEntity->getCollider().intersects(colliderEntity->getCollider().getUpBox(), this->collider.getDownBox()))
            {
                this->movement.momentum.y = 0;
                this->pos.y = (colliderEntity->getCollider().up() - this->collider.height());
                movement.grounded = true;
            }

            //smackin into roof
            if (this->movement.momentum.y < 0 && colliderEntity->getCollider().intersects(colliderEntity->getCollider().getDownBox(), this->collider.getUpBox()))
            {
                this->movement.momentum.y = 0;
                this->pos.y = (colliderEntity->getCollider().down());
            }

            if (colliderEntity->getCollider().intersects(colliderEntity->getCollider().getLeftBox(), this->collider.getRightBox()))
            {
                this->movement.momentum.x *= -0.5f;
                this->pos.x = (colliderEntity->getCollider().left() - this->collider.width());
            }

            if (colliderEntity->getCollider().intersects(colliderEntity->getCollider().getRightBox(), this->collider.getLeftBox()))
            {
                this->movement.momentum.x *= -0.5f;
                this->pos.x = (colliderEntity->getCollider().right());
            }
        }

        else if (colliderEntity->getCollider().hasComponent(ColliderKeys::grunt))
        {
            const Grunt* ptr = dynamic_cast<const Grunt*>(colliderEntity);
            if (ptr->isAttacking())
            {
                
                movement.addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider.getCenterPos(), colliderEntity->getCollider().getCenterPos(), movement.momentum, ptr->getMovementComp().momentum, movement.mass, ptr->getMovementComp().mass));
                healthComp.takeDamage(ptr->getDamage());
            }
        }
    }

    if (colliderEntity->getCollider().hasComponent(ColliderKeys::levelReturn))
        this->returning = true;

    else if (colliderEntity->getCollider().hasComponent(ColliderKeys::levelWarp))
        this->canReturn = true;

    else if (colliderEntity->getCollider().hasComponent(ColliderKeys::customTerrain))
    {
        std::string flag = colliderEntity->getCollider().getFlag();
        if (flag.compare(0, 4, "exit") == 0)
            this->exitReached = flag[4] - '0';
    }

    else if (colliderEntity->getCollider().hasComponent(ColliderKeys::throwable))
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
        int damage = explosion.calculateDamage(this->collider.getCenterPos());
        this->healthComp.takeDamage(damage);
    }
}

std::istream& operator>>(std::istream& in, Player& player)
{
    std::string trash;
    int health = 0;
    in >> trash;

    in >> trash >> player.movement.walkSpeed;
    in >> trash >> player.movement.jumpHeight;
    in >> trash >> player.movement.mass;
    in >> trash >> health;
    in >> trash >> player.throwingPower;

    player.healthComp.setMaxHealth(health);
    player.healthComp.setHealth(health);

    return in;
}
