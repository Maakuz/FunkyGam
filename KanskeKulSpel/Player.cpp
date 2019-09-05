#include "Player.h"
#include "LightQueue.h"

Player::Player(sf::Vector2f pos, int health, int speed, int attack)
:Entity(pos, health, speed, attack)
{
    this->lightBounceFreq = 100;
    this->lightBounceHeight = 30;
}

void Player::update(float dt)
{
    //testittesti
    static float elapsedTime = 0;
    elapsedTime += dt;

    Light light(getPosition() + sf::Vector2f(0, this->lightBounceHeight * sin(2 * 3.1415 * this->lightBounceFreq * elapsedTime + (720 - 720 / 4))), 100);
    LightQueue::get().queue(light);


}
