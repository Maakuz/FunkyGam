#pragma once
#include "SFML\Graphics.hpp"
#include <vector>

class CoordinateBar
{
public:
    CoordinateBar();
    void queueItems(sf::Vector2i min, sf::Vector2i max, sf::Vector2i offsetFromTile);

private:
    sf::RectangleShape xBar;
    sf::RectangleShape yBar;
    std::vector<sf::Text> numbers;
};

