#include "Recipe.h"
#include <sstream>

std::istream& operator>>(std::istream& in, Recipe& recipe)
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

    in >> trash;
    std::getline(in, trash);
    sstream.str(trash);

    while (!sstream.eof())
    {
        int componentID = 0;
        sstream >> componentID;
        recipe.components.push_back(componentID);
    }

    in >> trash;
    std::getline(in, trash);
    sstream.clear();
    sstream.str(trash);

    while (!sstream.eof())
    {
        int componentCount;
        sstream >> componentCount;
        recipe.componentAmounts.push_back(componentCount);
    }

    in >> trash >> recipe.resultID;
    in >> trash >> recipe.resultAmount;

    in >> trash;

    in.ignore();
    std::getline(in, descriptionLine);
    while (descriptionLine != "[DescriptionEnd]")
    {
        description.append(descriptionLine + "\n");
        std::getline(in, descriptionLine);
    }


    recipe.name.setString(name);
    recipe.description.setString(description);

    return in;
}
