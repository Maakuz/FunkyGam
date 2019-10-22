#include "HubHandler.h"
#include "TextureHandler.h"

HubHandler::HubHandler()
{
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background, states);
}
