#include "Spell.h"

Spell::Spell(sf::Vector2f pos):
    Collidable(pos, sf::Vector2f(32, 32))
{
    addComponent<TransformComp>(new TransformComp(pos));
}

std::istream& operator>>(std::istream& in, Spell& spell)
{
    std::string trash;
    in >> trash;
    std::getline(in, spell.name);
    while (spell.name.size() > 0 && spell.name[0] == ' ')
        spell.name.erase(spell.name.begin());

    spell.readSpecific(in);

    return in;
}

std::ostream& operator<<(std::ostream& out, const Spell& spell)
{
    out << "Name: " << spell.name << "\n";

    spell.writeSpecific(out);

    return out;
}
