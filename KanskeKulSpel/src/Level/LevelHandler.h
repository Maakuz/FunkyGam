#pragma once
#include "SFML/Graphics.hpp"
#include "Collision/CollisionHandler.h"
#include "Level/Terrain.h"
#include "Level/BreakableTerrain.h"
#include "Handlers/ShadowHandler.h"
#include "Misc/DebugDrawable.h"
#include "Lighting/Light.h"
#include "Level/Levels.h"
#include <vector>

#define NR_OF_LEVELS 1

class LevelHandler : public sf::Drawable, public DebugDrawable
{
public:
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

    bool loadLevel(Level level);

    void updateLevel(float dt);
    sf::Vector2i getDimensions() const { return this->dimensions; }

    std::vector<sf::Vector2f> generateEnemySpawnPoints();
    std::vector<sf::Vector2f> generateGatherPoints();
    std::vector<sf::Vector2f> generateRareGatherPoints();
    sf::Vector2f findPlayerSpawnPoint();
    const std::vector<Line>* getShadowLinePtr() const { return &this->shadowLines; }
    void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
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
    std::vector<BreakableTerrain> breakableTerrain;

    std::vector<Tilemap> tilemaps;

    bool drawCollision;

    bool importLevel(Level level);
    bool generateHitboxes(CollisionBox::ColliderKeys type);
    void generateShadowLines();
    void createSpites();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    

};