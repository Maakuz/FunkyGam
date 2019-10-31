#include "Player.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/VectorFunctions.h"
#include "Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Handlers/ItemHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Enemies/Grunt.h"

#define TESTING false

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos)
:MovingEntity(data, pos)
{
    this->collisionBox.addComponent(CollisionBox::ColliderKeys::player);
    this->collisionBox.addComponent(CollisionBox::ColliderKeys::character);
    this->walkSpeed = 0.05f;
    this->jumpHeight = 5.3f;
    this->mass = 0.166f;
    this->grounded = false;
    this->debugMode = false;
    this->noClip = false;
    this->ui = uiHandler;
    this->health = 100;
    this->maxHealth = 100;
    this->illumination = 0;
    this->jumping = false;

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

    if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Button::Right))
    {
        sf::Vector2f direction = mousePos - this->pos - sf::Vector2f(16, 10);
        normalize(direction);
        direction.x *= 8;
        direction.y *= 10;

        int itemID = this->ui->getInventory()->useSelectedItem();
        if (itemID != -1)
            ItemHandler::addThrowable(itemID, this->pos, direction, this);
    }

    for (int i = 0; i < 5; i++)
    {
        if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(27 + i))) //keys 1 to 5
        {
            ui->getInventory()->setSelectedItem(i);
        }
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::I))
    {
        if (ui->getInventory()->isInventoryOpen())
            ui->getInventory()->closeInventory();

        else
            ui->getInventory()->openInventory();
    }

    if (this->gatherableInRange != nullptr && KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::E))
    {
        this->gatherableInRange->pluck();
        this->ui->getInventory()->addItem(this->gatherableInRange->getID());
    }

    ui->setHealthPercentage(this->health / float(this->maxHealth));
}

void Player::reset(sf::Vector2f spawnPoint)
{
    setPosition(spawnPoint);
    this->returning = false;
    this->goalReached = false;
    this->health = this->maxHealth;
    this->momentum.x = 0;
    this->momentum.y = 0;
}

void Player::handleCollision(const Entity* collider)
{
    if (!noClip)
    {
        if (momentum.y > 0 && collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::platform))
        {
            //walking on ground
            if (collider->getCollisionBox().intersects(collider->getCollisionBox().getUp(), this->collisionBox.getDown()))
            {
                if (platformPassingCounter.isTimeUp())
                {
                    this->momentum.y = 0;
                    this->pos.y = collider->up() - this->height();
                    grounded = true;
                }
            }
        }

        else if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::grunt))
        {
            const Grunt* ptr = dynamic_cast<const Grunt*>(collider);
            if (ptr->isAttacking())
            {
                addCollisionMomentum(ptr->getMomentum(), ptr->getCenterPos(), ptr->getMass());
                health -= ptr->getDamage();
            }
        }

        MovingEntity::handleCollision(collider);
    }

    if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::levelReturn))
        this->returning = true;

    if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::levelEnd))
    {
        this->goalReached = true;
    }
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

std::istream& operator>>(std::istream& in, Player& player)
{
    std::string trash;
    while (trash != "[PlayerData]")
        in >> trash;

    in >> trash >> player.walkSpeed;
    in >> trash >> player.jumpHeight;
    in >> trash >> player.mass;
    in >> trash >> player.health;
    player.maxHealth = player.health;

    return in;
}
