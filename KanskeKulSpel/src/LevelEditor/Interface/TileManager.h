#pragma once
#include <vector>
#include "SFML/Graphics.hpp"
#include "Misc/Structs.h"

class TileManager
{
public:
    TileManager();
    virtual ~TileManager();


    void prepareTiles();
private:
    std::vector<sf::Drawable*> spriteQueue;
    std::vector<sf::Drawable*> toolSpriteQueue;

    sf::Drawable* processTile(const Tile& tile);
};