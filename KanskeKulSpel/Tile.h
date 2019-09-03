#pragma once
#include "SFML/Graphics.hpp"

class Tile : public sf::ConvexShape
{
public:
    Tile(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(80, 20));
    ~Tile();

    void setSize(sf::Vector2f size);
private:

};
