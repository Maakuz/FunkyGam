#include "Grid.h"

Grid::Grid(sf::Vector2i size, sf::Vector2f pos, sf::Vector2f tileSize)
{
    this->tileSize = tileSize;
    this->pos = pos;
    this->pos.x += (tileSize.x / 2);

    for (int i = 0; i < size.y; i++)
    {
        tiles.push_back(std::vector<Tile>());

        for (int j = 0; j < size.x; j++)
        {
            int xPos = pos.x + (j * tileSize.x / 2.f) - (i * tileSize.x / 2.f);
            int yPos = pos.y + (i * tileSize.y / 2.f) + (j * tileSize.y / 2.f);

            tiles[i].push_back(Tile(sf::Vector2f(xPos, yPos), tileSize));

            tiles[i][j].setFillColor(sf::Color::Green);
        }
    }
}

Grid::Grid(int xTiles, int yTiles, float tileSizeX, float tileSizeY, float xPos, float yPos)
    :Grid(sf::Vector2i(xTiles, yTiles), sf::Vector2f(tileSizeX, tileSizeY), sf::Vector2f(xPos, yPos))
{
}


Tile& Grid::getTile(sf::Vector2f pos)
{
   
    sf::Vector2f newPos = changeBase(pos);

    //Accounting for float errors maybe good 
    newPos.x = std::max(0.f, std::min(tiles[0].size() - 0.5f, newPos.x));
    newPos.y = std::max(0.f, std::min(tiles.size() - 0.5f, newPos.y));

    return this->tiles[(int)newPos.y][(int)newPos.x];

}

Tile& Grid::getTile(sf::Vector2i gridPos)
{
    if (gridPos.x >= this->tiles[0].size() || gridPos.y >= this->tiles.size())
    {
        printf("Outside tile array oooh you bad boy");
        return this->tiles[0][0];
    }
        return this->tiles[gridPos.y][gridPos.x];
}

sf::Vector2f Grid::getTileSize() const
{
    return this->tileSize;
}

sf::Vector2i Grid::getGridPos(sf::Vector2f pos) const
{
    sf::Vector2i xy = sf::Vector2i(changeBase(pos));

    xy.x = std::max(0, std::min((int)this->tiles[0].size() - 1, xy.x));
    xy.y = std::max(0, std::min((int)this->tiles.size() - 1, xy.y));

    return xy;
}

sf::Vector2f Grid::tileToWorldPos(sf::Vector2i tile) const
{
    if (this->tileExists(tile))
        return this->tiles[tile.y][tile.x].getPosition();
}

void Grid::highlightTile(sf::Vector2f pos)
{
    this->highlightTile(this->getGridPos(pos));
}

void Grid::highlightTile(sf::Vector2i gridPos)
{
    for (auto &tile : this->highlightedTiles)
        if (gridPos == tile)
            return; //Already highlighted

    this->highlightedTiles.push_back(gridPos);
    this->tiles[gridPos.y][gridPos.x].setFillColor(sf::Color::White);
}

void Grid::removeAllHighlights()
{
    for (auto& xy : this->highlightedTiles)
    {
        this->tiles[xy.y][xy.x].setFillColor(sf::Color::Green);
    }

    this->highlightedTiles.clear();
}

bool Grid::isInsideGrid(sf::Vector2i pos) const
{
    return isInsideGrid((sf::Vector2f)pos);
}

bool Grid::isInsideGrid(sf::Vector2f pos) const
{
    sf::Vector2f newPos = changeBase(pos);

    if (newPos.x < this->tiles[0].size() && 
        newPos.y < this->tiles.size() &&
        newPos.x >= 0 &&
        newPos.y >= 0)
        return true;
    
    return false;
}

bool Grid::tileExists(sf::Vector2i tile) const
{
    if (tile.x < 0 || tile.y < 0 || tile.y >= tiles.size() || tile.x >= tiles[0].size())
        return false;

    return true;
}

void Grid::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto &y : this->tiles)
        for (auto &x : y)
            target.draw(x, states);
}

sf::Vector2f Grid::changeBase(sf::Vector2f pos) const
{
    float x = (pos.x - this->pos.x);
    float y = (pos.y - this->pos.y);

    float x2 = (y / this->tileSize.y) + (x / this->tileSize.x);
    float y2 = (y / this->tileSize.y) - (x / this->tileSize.x);
    return sf::Vector2f(x2, y2);
}
