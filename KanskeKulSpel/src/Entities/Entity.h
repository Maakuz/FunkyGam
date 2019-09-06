#pragma once
#include "SFML/Graphics.hpp"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Vector2f pos, int health = 100, int speed = 10, int attack = 10);
    ~Entity() {};

    void setCollisionBox(sf::IntRect collisionBox);
    bool isCollisionEnabled() const;
    void enableCollision(bool boi);

protected:
    
    int health;
    int speed;

    int attack;

    bool collisionEnabled;
    
    sf::IntRect collissionBox;
};