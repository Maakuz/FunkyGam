#include "Player.h"
#include "Misc/Definitions.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/VectorFunctions.h"
#include "Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Handlers/ProjectileHandler.h"

#define TERMINALVELOCITY 10.f
#define TESTING false

Player::Player(AnimationData data, sf::Vector2f pos)
:AnimatedEntity(data, pos)
{
    this->lightBounceFreq = 100;
    this->lightBounceHeight = 30;

    this->acceleration = sf::Vector2i(0, 0);
    this->momentum = sf::Vector2f(0, 0);

    this->walkSpeed = 0.05f;
    this->floorRes = 0.85f;
    this->jumpHeight = 4.3f;
    this->mass = 0.166f;

    platformPassingCounter.stopValue = 1000;
    platformPassingCounter.counter = platformPassingCounter.stopValue;
}

void Player::update(float dt, sf::Vector2f mousePos)
{
    this->move(dt);
    this->updatePosition();

    this->updateAnimation(dt);

    if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Button::Left))
    {
        sf::Vector2f direction = mousePos - this->pos - sf::Vector2f(16, 10);
        normalize(direction);
        direction.x *= 8;
        direction.y *= 10;

        ProjectileHandler::addThrowable(TextureHandler::throwables::onlyBomb, this->pos, direction);
    }
    
    //imgui test
#if TESTING
    static int frame = 0;
    ImGui::Begin("turbotest");
    ImGui::SetWindowSize(sf::Vector2f(300, 300));
    ImGui::SliderFloat("Walkkk", &this->walkSpeed, 0, 5);
    ImGui::SliderFloat("floor res", &this->floorRes, 0, 1);
    ImGui::SliderFloat("jump", &this->jumpHeight, 0, 10);
    ImGui::SliderFloat("weight", &this->mass, 0, 5);
    ImGui::SliderInt("frame", &frame, 0, 6);
    if (ImGui::Button("Reset player pos")) this->pos = sf::Vector2f(0, 0);
    ImGui::End();

    //setAnimationFrame(sf::Vector2u(frame, 0));
#endif


}

void Player::handleCollision(const Entity& collider)
{
    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        //walking on ground
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y - this->size.y;
            grounded = true;
        }
        
        //smackin into roof
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y + collider.getSize().y;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.getPosition().x - this->size.x;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.getPosition().x + collider.getSize().x;
        }
    }

    else if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
    {
        //walking on ground
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            if (platformPassingCounter.isTimeUp())
            {
                this->momentum.y = 0;
                this->pos.y = collider.getPosition().y - this->size.y;
                grounded = true;
            }
        }
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
        {
            this->flipHorizontally();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        acceleration.x = -1;
        if (!this->isFlippedHorizontally())
        {
            this->flipHorizontally();
        }
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        jump();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        platformPassingCounter.reset();

    platformPassingCounter.update(dt);

    momentum.x += acceleration.x * walkSpeed * dt;
    momentum.x *= floorRes;
   
    momentum.y += GRAVITY * dt * this->mass;
    momentum.y = std::min(TERMINALVELOCITY, momentum.y);

    this->pos += momentum;

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


    momentum.x += acceleration.x * walkSpeed * dt;
    momentum.y += acceleration.y * walkSpeed * dt;
    momentum.x *= floorRes;
    momentum.y *= floorRes;

    //momentum.y += GRAVITY * dt * this->mass;
    //momentum.y = std::min(TERMINALVELOCITY, momentum.y);

    this->pos = momentum;
}

void Player::jump() // om sl�ppa knapp trycka ner spelar lite jappjapp
{
    if (this->grounded)
    {
        momentum.y = -jumpHeight;
        this->grounded = false;
    }
}
