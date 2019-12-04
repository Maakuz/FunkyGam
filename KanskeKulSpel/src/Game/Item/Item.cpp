#include "Game/Item/Item.h"

Item::Item(sf::Vector2f pos, const sf::Texture* texture)
{
    addComponent<LogisticsComp>(new LogisticsComp);

    SpriteComp* sprite = new SpriteComp(texture, pos);
    addComponent<SpriteComp>(sprite);
}

void Item::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getComponent<SpriteComp>(), states);
}

std::istream& operator>>(std::istream& in, Item& item)
{
    in >> *item.getComponent<LogisticsComp>();
    return in;
}

std::ostream& operator<<(std::ostream& out, const Item& item)
{
    out << *item.getComponent<LogisticsComp>();
    return out;
}
