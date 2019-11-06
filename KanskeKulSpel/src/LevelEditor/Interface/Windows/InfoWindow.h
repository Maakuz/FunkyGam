#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "LevelEditor/Constants.h"

class InfoWindow
{
public:
    InfoWindow() {};
    ~InfoWindow() {};

    void update();
    void fetchInfo(std::string fileName);
    void saveInfo(std::string fileName);
private:
    struct LevelInfo
    {
        std::string name;
        std::string description;
        int levelID;
        bool hasWarpPoint;
        std::string levelFileName;
        std::vector<int> gatherables;
        std::vector<int> rareGatherables;
        std::vector<int> unlockLevels;

        friend std::istream& operator>>(std::istream& in, LevelInfo& level)
        {
            std::string trash;
            std::string descriptionLine;
            std::stringstream sstream;

            in >> trash;
            in >> trash;
            std::getline(in, level.name);
            while (level.name.size() > 0 && level.name[0] == ' ')
                level.name.erase(level.name.begin());

            in >> trash >> level.levelFileName;
            in >> trash >> level.levelID;
            in >> trash >> level.hasWarpPoint;

            in >> trash;
            std::getline(in, trash);
            sstream.str(trash);

            while (!sstream.eof())
            {
                int unlock = 0;
                sstream >> unlock;
                level.unlockLevels.push_back(unlock);
            }

            in >> trash;
            std::getline(in, trash);
            sstream.clear();
            sstream.str(trash);

            while (!sstream.eof())
            {
                int itemID;
                sstream >> itemID;
                level.gatherables.push_back(itemID);
            }

            in >> trash;
            std::getline(in, trash);
            sstream.clear();
            sstream.str(trash);
            while (!sstream.eof())
            {
                int itemID;
                sstream >> itemID;
                level.rareGatherables.push_back(itemID);
            }

            in >> trash;

            in.ignore();
            std::getline(in, descriptionLine);
            while (descriptionLine != "[DescriptionEnd]")
            {
                level.description.append(descriptionLine + "\n");
                std::getline(in, descriptionLine);
            }

            return in;
        }
        friend std::ostream& operator<<(std::ostream& out, const LevelInfo& level)
        {
            std::string trash;
            std::string descriptionLine;
            std::stringstream sstream;

            out << "[" << level.levelFileName << "]\n";
            out << "levelName: " << level.name << "\n";
            out << "levelFileName: " << level.levelFileName << "\n";
            out << "levelID: " << level.levelID << "\n";
            out << "Warp: " << level.hasWarpPoint << "\n";
            out << "Unlocks:";
            for (int i : level.unlockLevels)
                out << " " << i;
            out << "\n";

            out << "Gatherables:";
            for (int i : level.gatherables)
                out << " " << i;
            out << "\n";

            out << "RareGatherables:";
            for (int i : level.rareGatherables)
                out << " " << i;
            out << "\n";


            out << "[Description]\n";
            out << level.description;
            out << "[DescriptionEnd]\n";

            return out;
        }

    };

    std::vector<LevelInfo> infos;
    int currentInfo;
};