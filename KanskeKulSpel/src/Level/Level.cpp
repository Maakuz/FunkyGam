#include "Level.h"

std::istream& operator>>(std::istream& in, LevelInfo& level)
{
    std::string trash;
    std::string name;
    std::string description;
    std::string descriptionLine;
    int gatherCount;
    int rareCount;

    in >> trash;
    in >> trash;
    std::getline(in, name);
    while (name.size() > 0 && name[0] == ' ')
        name.erase(name.begin());

    in >> trash >> level.levelFileName;
    in >> trash >> level.levelID;
    in >> trash >> level.unlockLevelID;
    in >> trash >> gatherCount;

    in >> trash;
    for (int i = 0; i < gatherCount; i++)
    {
        int itemID;
        in >> itemID;
        level.gatherables.push_back(itemID);
    }

    in >> trash >> rareCount;

    in >> trash;
    for (int i = 0; i < rareCount; i++)
    {
        int itemID;
        in >> itemID;
        level.rareGatherables.push_back(itemID);
    }

    in >> trash;

    in.ignore();
    std::getline(in, descriptionLine);
    while (descriptionLine != "[DescriptionEnd]")
    {
        description.append(descriptionLine + "\n");
        std::getline(in, descriptionLine);
    }


    level.name.setString(name);
    level.description.setString(description);

    return in;
}
