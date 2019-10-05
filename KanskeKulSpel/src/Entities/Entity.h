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

    virtual void flipHorizontally();

    ////ONLY USE THIS FROM THE OUTSIDE
    CollisionBox getCollisionBox() const { return this->collisionBox; };
    sf::Vector2f getSize() const { return this->size; };

    virtual void handleCollision(const Entity& collider) = 0;

protected:
    
    bool isFlippedHorizontally() const { return flipped; };
    void updatePosition();
    
    CollisionBox collisionBox;
    sf::Vector2f pos;
    sf::Vector2f size;
    
private:
    bool flipped;
};