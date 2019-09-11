#include "Player.h"
#include "Definitions.h"
#include "KeyboardState.h"
#include "Lighting/LightQueue.h"

#define PLAYER_WALK_SPEED 0.05f
#define AIR_RESISTANCE 0.9f
#define GROUND_RESISTANCE 0.9f
#define JUMP_HEIGHT 3

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

    this->move(dt);

    this->collisionBox.setPosition(getPosition());

}

void Player::handleCollision(const Entity& collider)
{
    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        //walking on ground
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            setPosition(getPosition().x, collider.getPosition().y - getTextureRect().height);
        }
        
        //smackin into roof
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            setPosition(getPosition().x, collider.getPosition().y + collider.getCollisionBox().getAABB().size.y);
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            setPosition(collider.getPosition().x - getTextureRect().width, getPosition().y);
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            setPosition(collider.getPosition().x + collider.getCollisionBox().getAABB().size.x, getPosition().y);
        }
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

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space))
        jump();


    momentum.x += acceleration.x * PLAYER_WALK_SPEED * dt;
    momentum.x *= GROUND_RESISTANCE;
   
    momentum.y += GRAVITY * dt;
    momentum.y *= AIR_RESISTANCE;


    setPosition(getPosition() + sf::Vector2f(momentum));

    this->updateAnimation(dt);
}

void Player::jump()
{
    sf::Vector2f currentPos = getPosition();
    setPosition(currentPos.x, currentPos.y - JUMP_HEIGHT);
    momentum.y -= JUMP_HEIGHT * 5;
}
