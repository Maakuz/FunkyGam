#include "Grid.h"

Grid::Grid(sf::Vector2i size, sf::Vector2f pos, sf::Vector2f tileSize)
{
    this->x = size.x;
    this->y = size.y;
    this->tileSize = tileSize;
    this->pos = pos;

    for (int i = 0; i < this->y; i++)
    {
        tiles.push_back(std::vector<Tile>());

        for (int j = 0; j < this->x; j++)
        {
            int xPos = pos.x + (j * tileSize.x / 2.f) - (i * tileSize.x / 2.f);
            int yPos = pos.y + (i * tileSize.y / 2.f) + (j * tileSize.y / 2.f);

            tiles[i].push_back(Tile(sf::Vector2f(xPos, yPos), tileSize));
        
            tiles[i][j].setFillColor(sf::Color(i * 20, j * 20, 0));
        }
    }

}

Grid::Grid(int xTiles, int yTiles, float tileSizeX, float tileSizeY, float xPos, float yPos)
    :Grid(sf::Vector2i(xTiles, yTiles), sf::Vector2f(tileSizeX, tileSizeY), sf::Vector2f(xPos, yPos))
{
}

Grid::~Grid()
{
}

Tile Grid::getTile(sf::Vector2f pos)
{
    return Tile();
}

Tile Grid::getTile(sf::Vector2i gridPos)
{
    return Tile();
}

void Grid::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto &y : this->tiles)
        for (auto &x : y)
            target.draw(x, states);

}
