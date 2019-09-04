#pragma once
#include "SFML/Graphics.hpp"
#include "Grid.h"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Texture* texture, sf::Vector2i pos, int health = 100, int speed = 10, int attack = 10); 
    Entity(sf::Vector2i pos, int health = 100, int speed = 10, int attack = 10);
    ~Entity() {};

    void setGridPos(sf::Vector2i pos);
    sf::Vector2i getGridPos() const;

    void placeOnGrid(const Grid& grid);

private:
    
    sf::Vector2i pos;

    int health;
    int speed;
    int attack;
};