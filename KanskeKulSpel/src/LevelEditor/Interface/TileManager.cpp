#include "TileManager.h"
#include "SFML\Graphics.hpp"
#include "LevelEditor/Constants.h"
#include "LevelEditor/Queues\TileQueue.h"
#include "LevelEditor/Queues\ToolTileQueue.h"
#include "LevelEditor/TileMaps.h"
#include "Renderer/Renderer.h"

const sf::Color HITBOX_COLORS[NR_OF_HITBOXES] = { 
    sf::Color(0, 0, 0, 150),
    sf::Color(100, 100, 100, 150),
    sf::Color(255, 0, 0, 150),
    sf::Color(255, 255, 0, 150),
    sf::Color(255, 130, 0, 150),
    sf::Color(0, 255, 0, 150),
    sf::Color(0, 255, 255, 150),
    sf::Color(0, 0, 255, 150),
    sf::Color(255, 0, 255, 150) };

const sf::Color SPECIAL_HITBOX_COLOR(255,255,255, 150);
    
TileManager::TileManager()
{
}

TileManager::~TileManager()
{
    for (size_t i = 0; i < spriteQueue.size(); i++)
        delete spriteQueue[i];

    for (size_t i = 0; i < toolSpriteQueue.size(); i++)
        delete toolSpriteQueue[i];
}

void TileManager::prepareTiles()
{
    for (size_t i = 0; i < spriteQueue.size(); i++)
        delete spriteQueue[i];


    for (size_t i = 0; i < toolSpriteQueue.size(); i++)
        delete toolSpriteQueue[i];

    spriteQueue.clear();
    toolSpriteQueue.clear();

    for (Tile tile : TileQueue::get().getQueue())
    {
        sf::Drawable* processedTile = processTile(tile);
        if (processedTile)
        {
            spriteQueue.push_back(processedTile);
            Renderer::queueDrawable(processedTile);
        }
    }

    for (Tile tile : ToolTileQueue::get().getQueue())
    {
        sf::Drawable* processedTile = processTile(tile);
        if (processedTile)
        {
            toolSpriteQueue.push_back(processedTile);
            Renderer::queueUI(processedTile);
        }
    }
}

sf::Drawable* TileManager::processTile(const Tile& tile)
{
    if (tile.tileID < HITBOX_ID_START)
    {
        sf::Sprite* sprite = new sf::Sprite;
        sprite->setTexture(TileMaps::get().getTexture(tile.textureID));
        sprite->setPosition((float)tile.x, (float)tile.y);
        sprite->setTextureRect(TileMaps::get().getTileRect(tile.textureID, tile.tileID));
        sprite->setColor(tile.color);
        return sprite;
    }

    else if (tile.tileID > HITBOX_ID_START&& tile.tileID < SPECIAL_HITBOX)
    {
        sf::RectangleShape* rect = new sf::RectangleShape;
        rect->setPosition((float)tile.x, (float)tile.y);
        rect->setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        rect->setFillColor(HITBOX_COLORS[tile.tileID - HITBOX_ID_START - 1]);
        return rect;
    }

    else if (tile.tileID == SPECIAL_HITBOX)
    {
        sf::RectangleShape* rect = new sf::RectangleShape;
        rect->setPosition((float)tile.x, (float)tile.y);
        rect->setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        rect->setFillColor(SPECIAL_HITBOX_COLOR);
        return rect;
    }

    return nullptr;
}
