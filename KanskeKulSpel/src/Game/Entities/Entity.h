#pragma once
#include "SFML/System/Vector2.hpp"

class Entity
{
public:
    Entity(sf::Vector2f pos)
    {
        this->pos = pos;
    }
    virtual ~Entity() {};

    sf::Vector2f getPosition() const { return pos; };

protected:

    sf::Vector2f pos;
};