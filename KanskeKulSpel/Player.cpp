#include "Player.h"

Player::Player(sf::Texture * texture, sf::Vector2i pos, int health, int speed, int attack)
:Entity(texture, pos, health, speed, attack)
{
}

Player::Player(sf::Vector2i pos, int health, int speed, int attack)
:Entity(pos, health, speed, attack)
{
}
