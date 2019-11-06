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

    LevelInfo()
    {
        levelID = -1;
    }

    friend std::istream& operator>>(std::istream& in, LevelInfo& level);
};