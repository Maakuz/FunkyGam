#pragma once
#include "SFML/Graphics.hpp"
#include <vector>

#define NR_OF_LEVELS 1

class LevelHandler : public sf::Drawable
{
public:
    enum levels 
    {
        forest = 0
    };

    struct Tilemap
    {
        sf::Texture texture;
        int x;
        int y;
    };

    struct Tile
    {
        int tileID;
        int textureID;
        int x;
        int y;
    };

    LevelHandler();
    virtual ~LevelHandler() {};

    bool loadLevel();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    bool importLevel(levels level);

    typedef std::vector<std::vector<Tile>> Layer;

    std::vector<Layer> layers;

    std::vector<Tilemap> tilemaps;
};