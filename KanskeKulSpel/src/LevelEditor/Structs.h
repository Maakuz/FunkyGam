#pragma once
#include <vector>
#include "Interactables\Button.h"

struct Tile
{
    Tile()
    {
        color = sf::Color::White;
    };
    int x;
    int y;
    //What texture.
    int textureID;
    //Where in the texture.
    int tileID;

    //an overlay color
    sf::Color color;
};

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

struct CustomHitbox
{
    sf::Vector2i min;
    sf::Vector2i max;
    std::string flag;

    friend std::ostream& operator<<(std::ostream& out, const CustomHitbox& obj)
    {
        out << obj.min.x << " " << obj.min.y << " ";
        out << obj.max.x << " " << obj.max.y << " ";
        out << obj.flag << "\n";

        return out;
    }

    friend std::istream& operator>>(std::istream& in, CustomHitbox& obj)
    {
        in >> obj.min.x >> obj.min.y;
        in >> obj.max.x >> obj.max.y;
        in >> obj.flag;

        return in;
    }

};
