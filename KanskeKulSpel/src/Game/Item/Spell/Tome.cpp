#include "Tome.h"

Tome::Tome(sf::Vector2f pos, const sf::Texture* texture)
    :Item(pos, texture)
{
}

std::istream& Tome::readSpecific(std::istream& in)
{
    std::string trash;

    in >> trash;
    in >> trash >> spell;

    return in;
}

std::ostream& Tome::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "SpellInvoked: " << spell << "\n";

    return out;
}
