#pragma once
#include "SFML/Graphics.hpp"
#include "Collision/CollisionHandler.h"
#include "Entities/Terrain.h"
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

    void updateLevel(float dt);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void drawCollision(sf::RenderWindow& window, sf::RenderStates states) const;

private:
    bool importLevel(levels level);
    bool generateHitboxes();
    void queueShadows();

    typedef std::vector<std::vector<Tile>> Layer;

    std::vector<Layer> layers;
    Layer hitboxData;

    std::vector<Terrain> terrain;

    std::vector<Tilemap> tilemaps;
};