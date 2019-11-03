#pragma once
#include <vector>
#include "Button.h"

//Coordianates are in relation to each other in tile-space...?
struct ActiveTile
{
    ActiveTile(Button::SIDES::SIDES_VISIBLE visibleSide = Button::SIDES::SIDES_VISIBLE::NONE)
    {
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineColor(sf::Color::White);
        box.setVisibleSide(visibleSide);
        box.setOutlineThickness(-2);
    };

    Button box;
    int tileID;
    int textureID;
    int x;
    int y;
};