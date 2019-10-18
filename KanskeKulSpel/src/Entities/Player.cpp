#include "Player.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/VectorFunctions.h"
#include "Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Handlers/ItemHandler.h"
#include "Misc/ConsoleWindow.h"

#define TESTING false

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos)
:MovingEntity(data, pos)
{
    this->collisionBox.addComponent(CollisionBox::colliderComponents::Player);
    this->collisionBox.addComponent(CollisionBox::colliderComponents::character);
    this->walkSpeed = 0.05f;
    this->jumpHeight = 5.3f;
    this->mass = 0.166f;
    this->grounded = false;
    this->debugMode = false;
    this->ui = uiHandler;
    this->selectedItemBarItem = 0;
    this->health = 100;

    platformPassingCounter.stopValue = 1000;
    platformPassingCounter.counter = platformPassingCounter.stopValue;

    this->inventory.quckslotItems[0] = 3;
    this->inventory.quckslotItems[1] = 0;
    this->inventory.quckslotItems[2] = 1;
    this->inventory.quckslotItems[3] = 2;
    this->inventory.quckslotItems[4] = -1;

    for (int i = 0; i < 5; i++)
    {
        ui->setItemSlot(i, this->inventory.quckslotItems[i]);
    }
    
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

    ConsoleWindow::get().addCommand("godmode", [&](Arguments args)->std::string
        {
            if (args.size() >= 1)
            {
                try
                {
                    int x = std::stoi(args[0]);

                    debugMode = x;

                    if (debugMode)
                        mass = 0;

                    else
                        mass = 0.166f;
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid argument 0 or 1.";
                    return ret;
                }
            }

            else
                return "missing argument 0 or 1";


            return "woosh";
        });

}

void Player::update(float dt, sf::Vector2f mousePos)
{
    if (!debugMode)
        this->move(dt);

    else
        this->debugMove(dt);

    MovingEntity::update(dt);

    if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Button::Right))
    {
        sf::Vector2f direction = mousePos - this->pos - sf::Vector2f(16, 10);
        normalize(direction);
        direction.x *= 8;
        direction.y *= 10;

        if (this->inventory.quckslotItems[selectedItemBarItem] != -1)
            ItemHandler::addThrowable(this->inventory.quckslotItems[selectedItemBarItem], this->pos, direction, this);
    }

    for (int i = 0; i < 5; i++)
    {
        if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(27 + i))) //keys 1 to 5
        {
            selectedItemBarItem = i;
            ui->setSelectedItem(i);
        }
    }

}

void Player::handleCollision(const Entity& collider)
{
    if (!debugMode)
    {
        if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
        {
            //walking on ground
            if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
            {
                if (platformPassingCounter.isTimeUp())
                {
                    this->momentum.y = 0;
                    this->pos.y = collider.up() - this->height();
                    grounded = true;
                }
            }
        }

        MovingEntity::handleCollision(collider);
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
        jump();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        platformPassingCounter.reset();

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

