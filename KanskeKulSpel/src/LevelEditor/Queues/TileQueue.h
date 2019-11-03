#pragma once
#include "misc/Structs.h"
#include <vector>

class TileQueue
{
public:
    static TileQueue & get()
    {
        static TileQueue queue;
        return queue;
    }
    virtual ~TileQueue() {};

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