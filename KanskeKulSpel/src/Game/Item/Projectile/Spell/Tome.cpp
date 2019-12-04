#include "Tome.h"
#include <fstream>

Tome::Tome(sf::Vector2f pos, const sf::Texture* texture)
{
    addComponent<SpriteComp>(new SpriteComp(texture, pos));
    addComponent<LogisticsComp>(new LogisticsComp);

    this->channelEmitterID = 0;
}

void Tome::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getComponent<SpriteComp>(), states);
}

std::istream& operator>>(std::istream& in, Tome& tome)
{
    std::string trash;

    in >> *tome.getComponent<LogisticsComp>();

    in >> trash;
    in >> trash >> tome.spell;
    in >> trash >> tome.channelEmitterID;

    return in;
}

std::ostream& operator<<(std::ostream& out, const Tome& tome)
{
    out << *tome.getComponent<LogisticsComp>();

    out << "[Specific]\n";
    out << "SpellInvoked: " << tome.spell << "\n";
    out << "channellID: " << tome.channelEmitterID << "\n";

    return out;
}
