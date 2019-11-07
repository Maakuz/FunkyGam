#pragma once
#include "Game/Interface/Info.h"
#include <vector>

struct LevelInfo : public Info
{
    int levelID;
    bool hasWarp;
    std::string levelFileName;
    std::vector<int> gatherables;
    std::vector<int> rareGatherables;
    std::vector<int> unlockLevels;
    std::vector<int> enemies;

    LevelInfo()
    {
        levelID = -1;
        hasWarp = false;
    }

    friend std::istream& operator>>(std::istream& in, LevelInfo& level);
};