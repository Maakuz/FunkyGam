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
    std::getline(in, name);
    while (name.size() > 0 && name[0] == ' ')
        name.erase(name.begin());

    in >> trash;
    std::getline(in, trash);
    sstream.str(trash);

    while (!sstream.eof() && trash.size() > 0)
    {
        int componentID = 0;
        sstream >> componentID;
        recipe.components.push_back(componentID);
    }

    in >> trash;
    std::getline(in, trash);
    sstream.clear();
    sstream.str(trash);

    while (!sstream.eof() && trash.size() > 0)
    {
        int componentCount;
        sstream >> componentCount;
        recipe.componentAmounts.push_back(componentCount);
    }

    in >> trash >> recipe.resultID;
    in >> trash >> recipe.resultAmount;
    in >> trash >> recipe.oneTimeRecipe;
    in >> trash >> recipe.unlocked;

    in >> trash;

    in.ignore();
    std::getline(in, descriptionLine);
    while (descriptionLine != "[DescriptionEnd]")
    {
        description.append(descriptionLine + "\n");
        std::getline(in, descriptionLine);
    }
    description.pop_back();

    recipe.name.setString(name);
    recipe.description.setString(description);

    return in;
}

std::ostream& operator<<(std::ostream& out, const Recipe& recipe)
{
    out << "RecipeName: " << std::string(recipe.name.getString()) << "\n";

    out << "ComponentID:";
    for (int i = 0; i < recipe.components.size(); i++)
    {
        out << " " << recipe.components[i];
    }

    out << "\nComponentAmount:";
    for (int i = 0; i < recipe.componentAmounts.size(); i++)
    {
        out << " " << recipe.componentAmounts[i];
    }

    out << "\nFinishedID: " << recipe.resultID << "\n";
    out << "FinishedCount: " << recipe.resultAmount << "\n";
    out << "OneTime: " << recipe.oneTimeRecipe << "\n";
    out << "startUnlocked: " << recipe.unlocked << "\n";
    

    out << "[Description]\n";
    out << std::string(recipe.description.getString()) << "\n";
    out << "[DescriptionEnd]";

    return out;
}
