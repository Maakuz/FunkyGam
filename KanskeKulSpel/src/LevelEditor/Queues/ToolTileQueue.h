#pragma once
#pragma once
#include "Misc/Structs.h"
#include <vector>

class ToolTileQueue
{
public:
    static ToolTileQueue& get()
    {
        static ToolTileQueue queue;
        return queue;
    }
    virtual ~ToolTileQueue() {};

    void queue(Tile tile)
    {
        tiles.push_back(tile);
    }

    void clear()
    {
        tiles.clear();
    }

    std::vector<Tile>& getQueue()
    {
        return tiles;
    }

private:
    std::vector<Tile> tiles;
};