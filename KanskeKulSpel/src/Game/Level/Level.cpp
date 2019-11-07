#include "Level.h"
#include <sstream>

std::istream& operator>>(std::istream& in, LevelInfo& level)
{
    std::string trash;
    std::string name;
    std::string description;
    std::string descriptionLine;
    std::stringstream sstream;

    in >> trash;
    in >> trash;
    std::getline(in, name);
    while (name.size() > 0 && name[0] == ' ')
        name.erase(name.begin());

    in >> trash >> level.levelFileName;
    in >> trash >> level.levelID;
    in >> trash >> level.hasWarp;
    
    in >> trash;
    std::getline(in, trash);
    sstream.str(trash);

    while (!sstream.eof() && trash.size() > 0)
    {
        int unlock = 0;
        sstream >> unlock;
        level.unlockLevels.push_back(unlock);
    }

    in >> trash;
    std::getline(in, trash);
    sstream.clear();
    sstream.str(trash);

    while (!sstream.eof() && trash.size() > 0)
    {
        int itemID;
        sstream >> itemID;
        level.gatherables.push_back(itemID);
    }

    in >> trash;
    std::getline(in, trash);
    sstream.clear();
    sstream.str(trash);
    while (!sstream.eof() && trash.size() > 0)
    {
        int itemID;
        sstream >> itemID;
        level.rareGatherables.push_back(itemID);
    }

    in >> trash;
    std::getline(in, trash);
    sstream.clear();
    sstream.str(trash);
    while (!sstream.eof() && trash.size() > 0)
    {
        int enemyID;
        sstream >> enemyID;
        level.enemies.push_back(enemyID);
    }

    in >> trash;

    in.ignore();
    std::getline(in, descriptionLine);
    while (descriptionLine != "[DescriptionEnd]")
    {
        description.append(descriptionLine + "\n");
        std::getline(in, descriptionLine);
    }
    description.pop_back();

    level.name.setString(name);
    level.description.setString(description);

    return in;
}
