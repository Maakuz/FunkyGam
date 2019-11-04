#pragma once
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"
#include <vector>

class TextureHandler
{
public:
    static TextureHandler& get()
    {
        static TextureHandler tex;
        return tex;
    }
    ~TextureHandler() {};

    void loadTextures();

    sf::Texture* getTexture(int texID);
    std::string getTextureName(int texID);
    const std::vector<sf::Texture>* getTextureVec()const { return &textures; };
    int getIDForTexture(const sf::Texture* texture);
    sf::Font* getFont() { return &font; };

private:
    std::vector<sf::Texture> textures;
    std::vector<std::string> texNames;
    sf::Font font;
};