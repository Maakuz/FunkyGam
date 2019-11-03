#pragma once
#include "SFML\Graphics.hpp"
#include "Interface\TileManager.h"

class TileStager
{
public:
    TileStager();
    virtual ~TileStager() {};

    void stageTiles();
    void clearAllQueues();

private:
    TileManager tileManager;
};