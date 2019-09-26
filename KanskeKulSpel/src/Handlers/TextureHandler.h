#pragma once
#include "SFML/Graphics/Texture.hpp"

#define TEXTURE_FOLDER "../Sprites/"
#define TEXTURE_PATH(x) TEXTURE_FOLDER x

class TextureHandler
{
public:
    static const int NR_OF_MISC = 2;
    enum misc
    {
        floorPiece = 0,
        playerSprite = 1,
    };

    static const int NR_OF_BOMBS = 1;
    enum bombs 
    {
        onlyBomb = 0
    };

    static TextureHandler& get()
    {
        static TextureHandler tex;
        return tex;
    }
    ~TextureHandler() {};

    void loadTextures()
    {
        if (!this->miscTextures[misc::floorPiece].loadFromFile(TEXTURE_PATH("floorPiece.png")))
            exit(-2);

        if (!this->miscTextures[misc::playerSprite].loadFromFile(TEXTURE_PATH("smallCate.png")))
            exit(-2);

        if (!this->bombsTextures[bombs::onlyBomb].loadFromFile(TEXTURE_PATH("bamb.png")))
            exit(-2);
    }

    sf::Texture* getTexture(misc tex)
    {
        return &this->miscTextures[tex];
    }

    sf::Texture* getTexture(bombs tex)
    {
        return &this->bombsTextures[tex];
    }

private:
    sf::Texture miscTextures[NR_OF_MISC];
    sf::Texture bombsTextures[NR_OF_BOMBS];
};