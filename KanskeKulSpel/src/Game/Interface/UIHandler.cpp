#include "UIHandler.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/Definitions.h"
#include "Game/Item/ItemHandler.h";
#include "Game/Misc/MouseState.h"
#include "Game/GameState.h"
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
    this->showEnemyHealth.stopValue = 5000;
    this->showEnemyHealth.counter = 5000;
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

    enemyHealthBarOverlay.setTexture(*TextureHandler::get().getTexture(11));

    healthBar[0].position = sf::Vector2f(0, HEALTH_BORDER_THICKNESS);
    healthBar[1].position = sf::Vector2f(healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    healthBar[2].position = sf::Vector2f(healthTexture->getSize().x, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);
    healthBar[3].position = sf::Vector2f(0, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);

    healthBar[0].texCoords = sf::Vector2f(0, HEALTH_BORDER_THICKNESS);
    healthBar[1].texCoords = sf::Vector2f(healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    healthBar[2].texCoords = sf::Vector2f(healthTexture->getSize().x, healthTexture->getSize().y- HEALTH_BORDER_THICKNESS);
    healthBar[3].texCoords = sf::Vector2f(0, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);

    enemyHealthBarOverlay.setPosition(WIN_WIDTH - enemyHealthBarOverlay.getTexture()->getSize().x, 0);
    sf::IntRect rect=  enemyHealthBarOverlay.getTextureRect();
    rect.left += rect.width;
    rect.width = -rect.width;
    enemyHealthBarOverlay.setTextureRect(rect);
    
    enemyHealthBar[0].position = sf::Vector2f(WIN_WIDTH, HEALTH_BORDER_THICKNESS);
    enemyHealthBar[1].position = sf::Vector2f(WIN_WIDTH - healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    enemyHealthBar[2].position = sf::Vector2f(WIN_WIDTH - healthTexture->getSize().x, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);
    enemyHealthBar[3].position = sf::Vector2f(WIN_WIDTH, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);

    enemyHealthBar[0].texCoords = sf::Vector2f(0, HEALTH_BORDER_THICKNESS);
    enemyHealthBar[1].texCoords = sf::Vector2f(healthTexture->getSize().x, HEALTH_BORDER_THICKNESS);
    enemyHealthBar[2].texCoords = sf::Vector2f(healthTexture->getSize().x, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);
    enemyHealthBar[3].texCoords = sf::Vector2f(0, healthTexture->getSize().y - HEALTH_BORDER_THICKNESS);


    view = sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT));
}

void UIHandler::update(float dt, sf::Vector2f mousePos)
{
    inventory.update(dt, mousePos);
    newItemDisplayDuration.update(dt);
    showEnemyHealth.update(dt);

    healthBar[1].position.x = HEALTH_START_TOP + (HEALTH_LENGTH * this->healthPercentage);
    healthBar[1].texCoords.x = HEALTH_START_TOP + (HEALTH_LENGTH * this->healthPercentage);

    healthBar[2].position.x = HEALTH_START_BOTTOM + (HEALTH_LENGTH * this->healthPercentage);
    healthBar[2].texCoords.x = HEALTH_START_BOTTOM + (HEALTH_LENGTH * this->healthPercentage);
}

void UIHandler::displayNewItem(int item)
{
    newItem.setText("New item: " + ItemHandler::getTemplate(item)->getLogisticsComp()->name + "!");
    newItem.resizeToFit();
    newItemDisplayDuration.reset();
}

void UIHandler::displayEnemyDamage(float percentage)
{
    this->showEnemyHealth.reset();
    enemyHealthBar[1].position.x = WIN_WIDTH - HEALTH_START_TOP - (HEALTH_LENGTH * percentage);
    enemyHealthBar[1].texCoords.x = HEALTH_START_TOP + (HEALTH_LENGTH * percentage);

    enemyHealthBar[2].position.x = WIN_WIDTH - HEALTH_START_BOTTOM - (HEALTH_LENGTH * percentage);
    enemyHealthBar[2].texCoords.x = HEALTH_START_BOTTOM + (HEALTH_LENGTH * percentage);
}

void UIHandler::hideEnemyDamage()
{
    showEnemyHealth.counter = showEnemyHealth.stopValue;
}

void UIHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(inventory, states);

    if (GameState::getState() == GameState::States::level)
    {
        states.texture = healthTexture;
        target.draw(&healthBar[0], 4, sf::PrimitiveType::Quads, states);
        target.draw(healthBarOverlay, states);

        if (!showEnemyHealth.isTimeUp())
        {
            target.draw(&enemyHealthBar[0], 4, sf::PrimitiveType::Quads, states);
            target.draw(enemyHealthBarOverlay, states);
        }
    }

    if (!newItemDisplayDuration.isTimeUp())
        target.draw(newItem, states);      
}
