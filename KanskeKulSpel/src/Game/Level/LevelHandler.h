#pragma once
#include "SFML/Graphics.hpp"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Level/Terrain.h"
#include "Game/Level/BreakableTerrain.h"
#include "Renderer/Lighting/ShadowHandler.h"
#include "Renderer/Lighting/Light.h"
#include "Renderer/DebugDrawable.h"
#include "Game/Level/Level.h"
#include "Game/Misc/BossSpawner.h"
#include "Misc/Structs.h"
#include <vector>

class LevelHandler : public sf::Drawable, public DebugDrawable
{
public:
    struct Tilemap
    {
        sf::Texture texture;
        int x;
        int y;
    };

    LevelHandler();
    virtual ~LevelHandler();

    bool loadLevel(const LevelInfo* level);
    void updateLevel(float dt);
    void queueColliders();
    void queueLightsAndShadows();

    bool isBossInLevel() const { return bossInLevel; };

    sf::Vector2i getDimensions() const { return this->dimensions; }

    std::vector<sf::Vector2f> generateEnemySpawnPoints();
    std::vector<sf::Vector2f> generateGatherPoints();
    std::vector<sf::Vector2f> generateRareGatherPoints();
    std::vector<CustomHitbox> getShrines();
    sf::Vector2f findPlayerSpawnPoints(int exitTaken);
    const std::vector<Line>* getShadowLinePtr() const { return &this->shadowLines; }
    const BossSpawner* getBossSpawnerPtr() const { return &this->bossSpawner; };
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
    std::vector<CustomHitbox> customHitboxes;

    std::vector<Line> shadowLines;
    std::vector<Terrain> terrain;

    std::vector<std::vector<Line>> breakableShadowLines;
    std::vector<BreakableTerrain> breakableTerrain;

    std::vector<Tilemap> tilemaps;

    bool drawCollision;
    bool bossInLevel;
    BossSpawner bossSpawner;

    bool importLevel(const LevelInfo* level);
    bool generateHitboxes(int id, ColliderKeys type);
    void generateShadowLines(const std::vector<Terrain>& terrain, std::vector<Line>* vec);
    void generateBreakableShadowLines();
    void createSpites();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void generateBossSpawner(const CustomHitbox& box);


};