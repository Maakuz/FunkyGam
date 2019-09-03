#pragma once
#include "Tile.h"
#include <vector>



class Grid : public sf::Drawable
{
public:
    Grid(sf::Vector2i size, sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f tileSize = sf::Vector2f(120, 30));
    Grid(int xTiles, int yTiles, float tileSizeX, float tileSizeY, float xPos = 0, float yPos = 0); //Not recommended
    ~Grid();

    Tile getTile(sf::Vector2f pos);
    Tile getTile(sf::Vector2i gridPos);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Vector2f pos;
    sf::Vector2f tileSize;
    int x;
    int y;
    std::vector<std::vector<Tile>> tiles;
};