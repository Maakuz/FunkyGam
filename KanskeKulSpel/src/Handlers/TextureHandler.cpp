#include "TextureHandler.h"
#include "Misc/Definitions.h"
#include <fstream>
#include <string>

void TextureHandler::loadTextures()
{
    std::fstream file(DATA_PATH "LoadList.mop");

    if (!file.is_open())
    {
        system("Pause");
        exit(-34);
    }

    std::string trash;
    std::string folderName;
    int textureCount;

    while (trash != "[Textures]")
        file >> trash;

    file >> trash >> folderName;
    file >> trash >> textureCount;

    for (int i = 0; i < textureCount; i++)
    {
        std::string fileName;
        file >> trash >> fileName;

        sf::Texture texture;
        if (!texture.loadFromFile(folderName + fileName))
        {
            system("Pause");
            exit(-65);
        }

        this->textures.push_back(texture);
    }
    std::string fontName;
    std::string fontFolder;
    file >> trash;
    file >> trash >> fontFolder;
    file >> trash >> fontName;

    if (!font.loadFromFile(fontFolder + fontName))
    {
        system("Pause");
        exit(-19);
    }


    file.close();
}

sf::Texture* TextureHandler::getTexture(int texID)
{
    return &this->textures[texID];
}
