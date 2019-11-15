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

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos)
    :MovingEntity(data, pos), spellComp(&this->pos)
{
    this->collider.addComponent(Collider::ColliderKeys::player);
    this->collider.addComponent(Collider::ColliderKeys::character);
    this->ui = uiHandler;
    this->walkSpeed = 0;
    this->jumpHeight = 0;
    this->mass = 0;
    this->health = 0;
    this->illumination = 0;
    this->maxHealth = 0;
    this->throwingPower = 0;
    this->exitReached = -1;
    this->debugMode = false;
    this->noClip = false;
    this->jumping = false;
    this->grounded = false;
    this->canReturn = false;
    this->returning = false;
    this->gatherableInRange = nullptr;

    platformPassingCounter.stopValue = 1000;
    platformPassingCounter.counter = platformPassingCounter.stopValue;
    
    static sf::Vector2f* posP = &this->pos; //testing
    ConsoleWindow::get().addCommand("setPos", [&](Arguments args)->std::string 
        {
            if (args.size() >= 2)
            {
                try
                {
                    int x = std::stoi(args[0]);
                    int y = std::stoi(args[1]);

                    posP->x = x;
                    posP->y = y;
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

                    health = x;
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

        ImGui::DragFloat("Walk speed", &this->walkSpeed, 0.01, 0, 10000);
        ImGui::DragFloat("Jump height", &this->jumpHeight, 0.01, 0, 10000);
        ImGui::DragFloat("Mass", &this->mass, 0.01, 0, 10000);

        ImGui::Checkbox("Noclip", &noClip);

        ImGui::End();
    }

    if (!noClip)
        this->move(dt);

    else
        this->debugMove(dt);

    if (this->grounded)
        setAnimation(0);

    else
        setAnimation(1);

    MovingEntity::update(dt);

    updateItems(dt, mousePos);
    spellComp.update(dt);


    if (this->canReturn && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        this->returning = true;

    if (this->gatherableInRange != nullptr && KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::E))
    {
        this->gatherableInRange->item.pluck();
        this->ui->getInventory()->addItem(this->gatherableInRange->item.getID(), this->gatherableInRange->count);
    }

    ui->setHealthPercentage(this->health / float(this->maxHealth));
    this->canReturn = false;
}

void Player::reset(sf::Vector2f spawnPoint, bool fillHealth)
{
    setPosition(spawnPoint);
    this->returning = false;
    this->exitReached = -1;
    this->momentum.x = 0;
    this->momentum.y = 0;

    if (fillHealth)
        this->health = this->maxHealth;
}

void Player::move(float dt)
{
    this->acceleration.x = 0;
    this->acceleration.y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->acceleration.x = 1;

        if (this->isFlippedHorizontally())
            this->flipHorizontally();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        acceleration.x = -1;

        if (!this->isFlippedHorizontally())
            this->flipHorizontally();
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        jump();
        this->jumping = true;
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        platformPassingCounter.reset();

    if (this->jumping && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        stopJump(0.3);


    platformPassingCounter.update(dt);

}

void Player::debugMove(float dt)
{
    this->acceleration.x = 0;
    this->acceleration.y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        this->acceleration.x = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        acceleration.x = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        acceleration.y = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        acceleration.y = 1;

    momentum.y = acceleration.y * dt;
    momentum.y *= 0.97;
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
                sf::Vector2f direction = mousePos - this->pos - sf::Vector2f(16, 10);
                float distance = std::min(length(direction), 400.f) / 400.f;
                normalize(direction);
                direction *= this->throwingPower * distance;

                ProjectileHandler::addThrowable(itemID, this->pos, direction, this);
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

void Player::handleCollision(const Entity* colliderEntity)
{
    if (!noClip)
    {
        if (momentum.y > 0 && colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::platform))
        {
            //walking on ground
            if (Collider::intersects(colliderEntity->getCollider().getUp(), this->collider.getDown()))
            {
                if (platformPassingCounter.isTimeUp())
                {
                    this->momentum.y = 0;
                    this->pos.y = colliderEntity->up() - this->height();
                    grounded = true;
                }
            }
        }

        else if (colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::grunt))
        {
            const Grunt* ptr = dynamic_cast<const Grunt*>(colliderEntity);
            if (ptr->isAttacking())
            {
                addCollisionMomentum(ptr->getMomentum(), ptr->getCenterPos(), ptr->getMass());
                health -= ptr->getDamage();
            }
        }

        MovingEntity::handleCollision(colliderEntity);
    }

    if (colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::levelReturn))
        this->returning = true;

    else if (colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::levelWarp))
        this->canReturn = true;

    else if (colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::customTerrain))
    {
        std::string flag = colliderEntity->getCollider().getFlag();
        if (flag.compare(0, 4, "exit") == 0)
            this->exitReached = flag[4] - '0';
    }

    else if (colliderEntity->getCollider().hasComponent(Collider::ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(colliderEntity);
        if (throwable->getThrower() != this)
            this->health -= throwable->getDamage();
    }
}

void Player::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(this->getCenterPos());
        this->health -= damage;
    }
}

std::istream& operator>>(std::istream& in, Player& player)
{
    std::string trash;
    while (trash != "[PlayerData]")
        in >> trash;

    in >> trash >> player.walkSpeed;
    in >> trash >> player.jumpHeight;
    in >> trash >> player.mass;
    in >> trash >> player.health;
    in >> trash >> player.throwingPower;
    player.maxHealth = player.health;

    return in;
}
