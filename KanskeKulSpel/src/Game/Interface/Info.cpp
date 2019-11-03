#include "Game/Interface/Info.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/Definitions.h"

Info::Info()
{
    name.setFont(*TextureHandler::get().getFont());
    name.setCharacterSize(DEFAULT_TEXT_SIZE);
    name.setFillColor(sf::Color::Black);
    description.setFont(*TextureHandler::get().getFont());
    description.setCharacterSize(DEFAULT_TEXT_SIZE);
    description.setFillColor(sf::Color::Black);
    unlocked = false;
    seen = false;
}

void Info::setFillColor(sf::Color color)
{
    name.setFillColor(color);
    description.setFillColor(color);
}

void Info::setOutlineColor(sf::Color color)
{
    name.setOutlineColor(color);
    description.setOutlineColor(color);
}

void Info::setCharacterSize(int size)
{
    name.setCharacterSize(size);
    description.setCharacterSize(size);
}
