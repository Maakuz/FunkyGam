#pragma once
#include "SFML/Graphics/Texture.hpp"

#define TEXTURE_FOLDER "../Sprites/"
#define TEXTURE_PATH(x) TEXTURE_FOLDER x

class TextureHandler
{
public:
    static const int NR_OF_CHARACTERS = 2;
    enum characters
    {
        player = 0,
        grunt = 1
    };

    static const int NR_OF_BOMBS = 3;
    enum throwables 
    {
        bomb = 0,
        rock = 1,
        flare = 2
    };

    static const int NR_OF_UI_ELEMENTS = 1;
    enum ui 
    {
        quickSlot = 0
    };

    static TextureHandler& get()
    {
        static TextureHandler tex;
        return tex;
    }
    ~TextureHandler() {};

    void loadTextures()
    {
        if (!this->miscTextures[characters::player].loadFromFile(TEXTURE_PATH("smallCate.png")))
            exit(-2);

        if (!this->miscTextures[characters::grunt].loadFromFile(TEXTURE_PATH("fish.png")))
            exit(-2);

        if (!this->projectileTextures[throwables::bomb].loadFromFile(TEXTURE_PATH("bamb.png")))
            exit(-2);

        if (!this->projectileTextures[throwables::rock].loadFromFile(TEXTURE_PATH("rock.png")))
            exit(-2);

        if (!this->projectileTextures[throwables::flare].loadFromFile(TEXTURE_PATH("flare.png")))
            exit(-2);

        if (!this->uiTextures[ui::quickSlot].loadFromFile(TEXTURE_PATH("/UI/ui_slot.png")))
            exit(-2);
    }

    sf::Texture* getTexture(characters tex)
    {
        return &this->miscTextures[tex];
    }

    sf::Texture* getTexture(throwables tex)
    {
        return &this->projectileTextures[tex];
    }

    sf::Texture* getTexture(ui tex)
    {
        return &this->uiTextures[tex];
    }

private:
    sf::Texture miscTextures[NR_OF_CHARACTERS];
    sf::Texture projectileTextures[NR_OF_BOMBS];
    sf::Texture uiTextures[NR_OF_UI_ELEMENTS];
};