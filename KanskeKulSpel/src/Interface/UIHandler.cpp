#include "UIHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/Definitions.h"
#include "Handlers/ItemHandler.h";
#include "Misc/MouseState.h"
#include "GameState.h"
#include <fstream>

#define HEALTH_START_BOTTOM 15
#define HEALTH_START_TOP 69
#define HEALTH_LENGTH 398
#define HEALTH_BORDER_THICKNESS 10

UIHandler::UIHandler()
{
    this->slotTexture = nullptr;
    this->healthTexture = nullptr;
    this->healthPercentage = 1;
    this->newItem.create(TextureHandler::get().getTexture(7), TextureHandler::get().getFont());
    this->newItem.setPos(sf::Vector2f(0, WIN_HEIGHT - 100));
    this->newItem.setCharacterSize(14);
    this->newItemDisplayDuration.stopValue = 5000;
    this->newItemDisplayDuration.counter = 5000;
}

UIHandler::~UIHandler()
{

}

void UIHandler::initialize()
{
    this->slotTexture = TextureHandler::get().getTexture(5);
    this->slotSize = slotTexture->getSize();
    this->slotSize.x /= 2;
    inventory.initialize(this->slotTexture);

    healthTexture = TextureHandler::get().getTexture(10);
    healthBarOverlay.setTexture(*TextureHandler::get().getTexture(11));

    healthBar[0].position = sf::Vector2f(0, HEALTH_BORDER_THICKNESS);
    healthBar[1].position = sf::Vector2f(healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    healthBar[2].position = sf::Vector2f(healthTexture->getSize().x, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);
    healthBar[3].position = sf::Vector2f(0, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);

    healthBar[0].texCoords = sf::Vector2f(0, HEALTH_BORDER_THICKNESS);
    healthBar[1].texCoords = sf::Vector2f(healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    healthBar[2].texCoords = sf::Vector2f(healthTexture->getSize().x, healthTexture->getSize().y- HEALTH_BORDER_THICKNESS);
    healthBar[3].texCoords = sf::Vector2f(0, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);


    view = sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT));
}

void UIHandler::update(float dt, sf::Vector2f mousePos)
{
    inventory.update(dt, mousePos);
    newItemDisplayDuration.update(dt);

    healthBar[1].position.x = HEALTH_START_TOP + (HEALTH_LENGTH * this->healthPercentage);
    healthBar[1].texCoords.x = HEALTH_START_TOP + (HEALTH_LENGTH * this->healthPercentage);

    healthBar[2].position.x = HEALTH_START_BOTTOM + (HEALTH_LENGTH * this->healthPercentage);
    healthBar[2].texCoords.x = HEALTH_START_BOTTOM + (HEALTH_LENGTH * this->healthPercentage);
}

void UIHandler::displayNewItem(int item)
{
    newItem.setText("New item: " + ItemHandler::getTemplate(item)->getName() + "!");
    newItem.resizeToFit();
    newItemDisplayDuration.reset();
}

void UIHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(inventory, states);

    if (GameState::getState() == GameState::States::level)
    {
        states.texture = healthTexture;
        target.draw(&healthBar[0], 4, sf::PrimitiveType::Quads, states);
        target.draw(healthBarOverlay, states);
    }

    if (!newItemDisplayDuration.isTimeUp())
        target.draw(newItem, states);      
}
