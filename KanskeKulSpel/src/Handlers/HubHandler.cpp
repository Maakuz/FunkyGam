#include "HubHandler.h"
#include "TextureHandler.h"

HubHandler::HubHandler()
{
    this->background.setTexture(*TextureHandler::get().getTexture(6));
}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}
