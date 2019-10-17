#pragma once
#include "SFML/Graphics/Texture.hpp"
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

private:
    std::vector<sf::Texture> textures;
};