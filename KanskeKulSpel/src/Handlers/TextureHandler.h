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
    sf::Font* getFont() { return &font; };

private:
    std::vector<sf::Texture> textures;
    sf::Font font;
};