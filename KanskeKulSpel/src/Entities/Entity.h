#pragma once
#include "SFML/Graphics.hpp"
#include "Collision/CollisionBox.h"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Vector2f pos, sf::Texture * texture);
    ~Entity() {};

    void addCollision(CollisionBox::AABB aabb);


    //Uses textureRect to determinate bounds
    void addCollision();

    ////ONLY USE THIS FROM THE OUTSIDE
    CollisionBox getCollisionBox() const { return this->collisionBox; };

    virtual void handleCollision(const Entity& collider) = 0;

protected:
    
    void updatePosition();
    
    CollisionBox collisionBox;
    sf::Vector2f pos;
};