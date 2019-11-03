#pragma once
#include "SFML/Graphics/Text.hpp"

struct Info
{
    sf::Text name;
    sf::Text description;
    bool unlocked;
    bool seen;
    Info();

    void setFillColor(sf::Color color);

    void setOutlineColor(sf::Color color);

    void setCharacterSize(int size);
};
