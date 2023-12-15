#include "CoordinateBar.h"
#include "LevelEditor/Constants.h"
#include "LevelEditor/Queues\MiscQueue.h"
#include "Game/Handlers/TextureHandler.h"

CoordinateBar::CoordinateBar()
{
    xBar.setPosition(TILEMENU_WIDTH + COORDINATE_BAR_SIZE, MENU_BAR_HEIGHT - 1);
    yBar.setPosition(TILEMENU_WIDTH, MENU_BAR_HEIGHT + COORDINATE_BAR_SIZE - 1);

    xBar.setFillColor({ 20, 20, 20, 255 });
    yBar.setFillColor({ 20, 20, 20, 255 });
}

void CoordinateBar::queueItems(sf::Vector2i min, sf::Vector2i max, sf::Vector2i offsetFromTile)
{    
    this->xBar.setSize(sf::Vector2f((max.x - min.x) * TILE_SIZE, COORDINATE_BAR_SIZE));
    this->yBar.setSize(sf::Vector2f(COORDINATE_BAR_SIZE, (max.y - min.y) * TILE_SIZE));

    MiscQueue::get().queue(this->xBar);
    MiscQueue::get().queue(this->yBar);

    for (int i = min.x; i < max.x; i++)
    {
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(1);
        rect.setSize({ TILE_SIZE, COORDINATE_BAR_SIZE });
        rect.setPosition(sf::Vector2f(((i * TILE_SIZE) + offsetFromTile.x + xBar.getPosition().x) - ((min.x * TILE_SIZE) - offsetFromTile.x), xBar.getPosition().y));

        sf::Text text(std::to_string(i), *TextureHandler::get().getFont(), 16);
        int middleOffset = (TILE_SIZE / 2) - (text.getGlobalBounds().width / 2);
        text.setPosition(sf::Vector2f(((i * TILE_SIZE) + offsetFromTile.x + xBar.getPosition().x) - ((min.x * TILE_SIZE) + offsetFromTile.x) + middleOffset, xBar.getPosition().y));

        text.setFillColor(sf::Color::White);

        MiscQueue::get().queue(rect);
        MiscQueue::get().queue(text);
    }
}