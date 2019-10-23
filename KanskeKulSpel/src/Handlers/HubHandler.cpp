#include "HubHandler.h"
#include "TextureHandler.h"
#include "SFML/Window/Mouse.hpp"

#define OFF_TEX 8
#define ON_TEX 9

HubHandler::HubHandler()
{
    this->levelSelected = -1;
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
    this->button.create(TextureHandler::get().getTexture(8));
    this->button.setFont(TextureHandler::get().getFont());
    this->button.setCharacterSize(16);
    this->button.setText("Play level 1");
    this->button.resizeToFit();
    this->button.setPos(sf::Vector2f(600, 100));
}

void HubHandler::update(float dt, sf::Vector2f mousePos)
{
    if (button.getBounds().contains(mousePos))
    {
        this->button.setTexture(TextureHandler::get().getTexture(ON_TEX));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            levelSelected = 0;
        }
    }

    else
        this->button.setTexture(TextureHandler::get().getTexture(OFF_TEX));

}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background, states);
    target.draw(button, states);
}
