#pragma once
#include "Tile.h"
#include <vector>



class Grid : public sf::Drawable
{
public:
    Grid(sf::Vector2i size, sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f tileSize = sf::Vector2f(120, 30));
    Grid(int xTiles, int yTiles, float tileSizeX, float tileSizeY, float xPos = 0, float yPos = 0); //Not recommended
    ~Grid();

    Tile& getTile(sf::Vector2f pos);
    Tile& getTile(sf::Vector2i gridPos);

    sf::Vector2i getGridPos(sf::Vector2f pos) const;

    void highlightTile(sf::Vector2f pos);
    void highlightTile(sf::Vector2i gridPos);

    void removeAllHighlights();

    bool isInsideGrid(sf::Vector2i pos) const;
    bool isInsideGrid(sf::Vector2f pos) const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Vector2f changeBase(sf::Vector2f pos) const;

    sf::Vector2f pos;
    sf::Vector2f tileSize;
    std::vector<std::vector<Tile>> tiles;
    std::vector<sf::Vector2i> highlightedTiles;


};