#pragma once
#include "SFML/Graphics.hpp"
#include "Collision/CollisionHandler.h"
#include "Entities/Terrain.h"
#include "ShadowHandler.h"
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
    bool generateHitboxes(CollisionBox::colliderComponents type);
    void generateShadowLines();
    void createSpites();

    typedef std::vector<std::vector<Tile>> Layer;
    typedef std::vector<std::vector<sf::Sprite>> SpriteLayer;

    std::vector<Layer> layers;
    std::vector<SpriteLayer> spriteLayers;
    std::vector<sf::Sprite> linearSprite;
    Layer hitboxData;

    std::vector<ShadowHandler::Line> shadowLines;

    std::vector<Terrain> terrain;

    std::vector<Tilemap> tilemaps;
};