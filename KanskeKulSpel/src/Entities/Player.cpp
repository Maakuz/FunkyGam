#include "Player.h"
#include "Lighting/LightQueue.h"
#include "KeyboardState.h"

#define WALK_SPEED 0.5f

Player::Player(AnimationData data, sf::Vector2f pos)
:AnimatedEntity(data, pos)
{
    this->lightBounceFreq = 100;
    this->lightBounceHeight = 30;

    this->acceleration = sf::Vector2i(0, 0);
    this->momentum = sf::Vector2f(0, 0);
    this->friction = 0.9f;
}

void Player::update(float dt)
{
    //testittesti
    static float elapsedTime = 0;
    elapsedTime += dt;

    Light light(getPosition() + sf::Vector2f(0, this->lightBounceHeight * sin(2 * 3.1415 * this->lightBounceFreq * elapsedTime + (720 - 720 / 4))), 100);
    LightQueue::get().queue(light);

    this->move(dt);
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

    momentum.x += acceleration.x * WALK_SPEED * dt;
    momentum.x *= friction;


    setPosition(getPosition() + sf::Vector2f(momentum));

    this->updateAnimation(dt);
}
