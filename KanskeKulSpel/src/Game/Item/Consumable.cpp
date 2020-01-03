#include "Consumable.h"


Consumable::Consumable(sf::Vector2f pos, const sf::Texture* texture)
{
    SpriteComp* sprite = new SpriteComp(texture, pos);
    addComponent<SpriteComp>(sprite);
    addComponent<LogisticsComp>(new LogisticsComp);
    addComponent<StatusComp>(new StatusComp);
}

void Consumable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getSpriteComp(), states);
}

std::istream& operator>>(std::istream& in, Consumable& item)
{
    in >> *item.getComponent<LogisticsComp>();
    in >> *item.getComponent<StatusComp>();
    return in;
}

std::ostream& operator<<(std::ostream& out, const Consumable& item)
{
    out << *item.getComponent<LogisticsComp>();
    out << *item.getComponent<StatusComp>();
    return out;
}
