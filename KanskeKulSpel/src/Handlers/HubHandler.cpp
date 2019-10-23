#include "HubHandler.h"
#include "TextureHandler.h"

HubHandler::HubHandler()
{
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
    this->button.setTexture(TextureHandler::get().getTexture(7));
    this->button.setFont(TextureHandler::get().getFont());
    this->button.setCharacterSize(16);

    button.setWidth(200);
    button.setPos(sf::Vector2f(100, 100));
    button.setHeight(400);
}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background, states);
    target.draw(button, states);
}
