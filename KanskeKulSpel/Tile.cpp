#include "Tile.h"

Tile::Tile(sf::Vector2f pos, sf::Vector2f size)
    :sf::ConvexShape(4)
{
    setPosition(pos);

    setPoint(0, sf::Vector2f(size.x / 2.f, 0));
    setPoint(1, sf::Vector2f(size.x, size.y / 2.f));
    setPoint(2, sf::Vector2f(size.x / 2.f, size.y));
    setPoint(3, sf::Vector2f(0, size.y / 2.f));
}

Tile::~Tile()
{
}

void Tile::setSize(sf::Vector2f size)
{
    setPoint(0, sf::Vector2f(size.x / 2.f, 0));
    setPoint(1, sf::Vector2f(size.x, size.y / 2.f));
    setPoint(2, sf::Vector2f(size.x / 2.f, size.y));
    setPoint(3, sf::Vector2f(0, size.y / 2.f));
}