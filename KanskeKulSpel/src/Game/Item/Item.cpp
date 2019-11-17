#include "Game/Item/Item.h"

Item::Item(sf::Vector2f pos, const sf::Texture* texture):
    Entity(pos),
    sprite(texture, pos)
{
    id = 0;
    stackLimit = 0;
    emitterID = -1;
    useable = false;
    obtained = false;
}

void Item::setPosition(sf::Vector2f pos)
{
    this->pos = pos;
    this->sprite.update(pos);
}

void Item::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}

std::istream& operator>>(std::istream& in, Item& item)
{
    std::string trash;
    in >> trash;
    std::getline(in, item.name);
    while (item.name.size() > 0 && item.name[0] == ' ')
        item.name.erase(item.name.begin());

    in >> trash >> item.stackLimit;
    in >> trash >> item.emitterID;
    in >> trash >> item.useable;

    item.readSpecific(in);

    return in;
}

std::ostream& operator<<(std::ostream& out, const Item& item)
{
    out << "Name: " << item.name << "\n";
    out << "StackLimit: "<< item.stackLimit << "\n";
    out << "EmitterID: "<< item.emitterID << "\n";
    out << "Useable: " << item.useable << "\n";

    item.writeSpecific(out);

    return out;
}
