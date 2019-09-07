#pragma once
#include "Entity.h"

class Tile : public Entity
{
public:
    Tile(sf::Vector2f pos, sf::Texture * texture);
    ~Tile() {};



    virtual void handleCollision(const Entity & collider) {};
private:

};