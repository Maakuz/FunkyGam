#include "Player.h"
#include "Definitions.h"
#include "KeyboardState.h"
#include "Lighting/LightQueue.h"

#define PLAYER_WALK_SPEED 0.1f
#define AIR_RESISTANCE 0.8f
#define GROUND_RESISTANCE 0.9f

Player::Player(AnimationData data, sf::Vector2f pos)
:AnimatedEntity(data, pos)
{
    this->lightBounceFreq = 100;
    this->lightBounceHeight = 30;

    this->acceleration = sf::Vector2i(0, 0);
    this->momentum = sf::Vector2f(0, 0);
}

void Player::update(float dt)
{
    //testittesti
    static float elapsedTime = 0;
    elapsedTime += dt;

    Light light(getPosition() + sf::Vector2f(0, this->lightBounceHeight * sin(2 * 3.1415 * this->lightBounceFreq * elapsedTime + (720 - 720 / 4))), 100);
    LightQueue::get().queue(light);

    this->collisionBox.setPosition(getPosition());

    this->move(dt);
}

void Player::handleCollision(const Entity& collider)
{
    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        this->momentum.y = 0;
        setPosition(getPosition().x, collider.getPosition().y - getTextureRect().height);
    }
}

void Player::move(float dt)
{
    this->acceleration.x = 0;
    this->acceleration.y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        this->acceleration.x = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        acceleration.x = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        acceleration.y = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        acceleration.y = -1;

    momentum.x += acceleration.x * PLAYER_WALK_SPEED * dt;
    momentum.x *= GROUND_RESISTANCE;
   
    momentum.y += GRAVITY * dt;
    momentum.y *= AIR_RESISTANCE;


    setPosition(getPosition() + sf::Vector2f(momentum));

    this->updateAnimation(dt);
}
