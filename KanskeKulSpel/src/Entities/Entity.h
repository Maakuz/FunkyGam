#pragma once
#include "SFML/Graphics.hpp"
#include "CollisionBox.h"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Vector2f pos, sf::Texture * texture);
    ~Entity() {};

    void addCollision(CollisionBox::AABB aabb);

    //Uses textureRect to determinate bounds
    void addCollision();

    ////ONLY USE THIS FROM THE OUTSIDE
    CollisionBox getCollisionBox() { return this->collisionBox; };

    //Todo: Make pure virtual as soon as floors are done
    virtual void handleCollision(CollisionBox const & collider) 
    {

    } 

protected:
    CollisionBox collisionBox;
};