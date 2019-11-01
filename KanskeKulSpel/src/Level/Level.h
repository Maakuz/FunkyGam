#pragma once
#include "Interface/Info.h"
#include <vector>

struct LevelInfo : public Info
{
    int levelID;
    int unlockLevelID;
    std::string levelFileName;
    std::vector<int> gatherables;
    std::vector<int> rareGatherables;

    LevelInfo()
    {
        levelID = -1;
        unlockLevelID = -1;
    }

    friend std::istream& operator>>(std::istream& in, LevelInfo& level);
};