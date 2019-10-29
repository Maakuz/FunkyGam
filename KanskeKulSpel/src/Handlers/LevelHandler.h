#pragma once
#include "SFML/Graphics.hpp"
#include "Collision/CollisionHandler.h"
#include "Entities/Terrain.h"
#include "ShadowHandler.h"
#include "Lighting/Light.h"
#include <vector>

#define NR_OF_LEVELS 1

class LevelHandler : public sf::Drawable
{
public:
    enum Levels 
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
    virtual ~LevelHandler();

    bool loadLevel(Levels level);

    void updateLevel(float dt);
    sf::Vector2i getDimensions() const { return this->dimensions; }

    std::vector<sf::Vector2f> generateSpawnPoints();
    std::vector<sf::Vector2f> generateGatherPoints();
    void drawCollision(sf::RenderTarget& target, sf::RenderStates states) const;
    const std::vector<Line>* getShadowLinePtr() const { return &this->shadowLines; }
private:

    bool importLevel(Levels level);
    bool generateHitboxes(CollisionBox::ColliderKeys type);
    void generateShadowLines();
    void createSpites();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    
    typedef std::vector<std::vector<Tile>> Layer;

    std::vector<Layer> layers;
    std::vector<sf::Sprite> linearSprite;
    std::vector<Light*> lights;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Vector2i dimensions;

    Layer hitboxData;

    std::vector<Line> shadowLines;

    std::vector<Terrain> terrain;

    std::vector<Tilemap> tilemaps;
};