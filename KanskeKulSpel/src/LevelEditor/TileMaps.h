#pragma once
#include <vector>
#include "SFML\Graphics\Texture.hpp"
#include <assert.h>
#include "Constants.h"
#include <fstream>

class TileMaps
{
public:
    static TileMaps & get()
    {
        static TileMaps resources;
        return resources;
    }
    virtual ~TileMaps() {};

    friend std::ostream & operator<<(std::ostream & out, const TileMaps & tileMaps);
    friend std::istream & operator>>(std::istream & in, TileMaps & tileMaps);

    //Saving a pointer to this could be dangerous!!
    sf::Texture & getTexture(int id);
   

    int getTextureIndex(std::string name);
    int getTexureCount() const { return (int)textureNames.size(); };
    const std::vector<std::string>& getTextureNameList() const { return textureNames; };

    sf::IntRect & getTileRect(int textureID, int tileID);
    sf::Vector2i getSheetSize(int textureID);

    //Width and height is only to determinate size if texture loading fails
    void addTexture(std::string newName, int width = 0, int height = 0);

private:
    std::vector<sf::Texture> textures;
    std::vector<std::string> textureNames;
    std::vector<sf::Vector2i> sheetSize;
    std::vector<std::vector<sf::IntRect>> textureRects;
};