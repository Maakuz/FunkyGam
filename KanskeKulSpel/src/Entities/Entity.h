#pragma once
#include "SFML/Graphics.hpp"
#include "CollisionBox.h"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Vector2f pos, sf::Texture * texture);
    Entity(sf::Vector2f pos);
    ~Entity() {};

    void addCollision(CollisionBox::AABB aabb);

    //Uses textureRect to determinate bounds
    void addCollision();


protected:
    CollisionBox collisionBox;
};