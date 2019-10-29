#include "Entities/Items/Item.h"

Item::Item(sf::Vector2f pos, sf::Texture* texture):
    Entity(pos, texture)
{
    id = 0;
    stackLimit = 0;
    emitterID = -1;
    useable = false;
    obtainable = false;
    obtained = false;
}

bool Item::pluck()
{
    if (this->obtainable)
        this->obtained = true;

    return this->obtained;
}

std::istream& operator>>(std::istream& in, Item& item)
{
    std::string trash;
    in >> trash >> item.name;
    in >> trash >> item.stackLimit;
    in >> trash >> item.emitterID;
    in >> trash >> item.useable;

    return in;
}