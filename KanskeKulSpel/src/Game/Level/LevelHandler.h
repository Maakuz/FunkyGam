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

    bool isBossInLevel() const { return m_bossInLevel; };

    sf::Vector2i getDimensions() const { return m_dimensions; }

    std::vector<sf::Vector2f> generateEnemySpawnPoints();
    std::vector<sf::Vector2f> generateGatherPoints();
    std::vector<sf::Vector2f> generateRareGatherPoints();
    std::vector<CustomHitbox> getShrines();
    sf::Vector2f findPlayerSpawnPoints(int exitTaken);
    const std::vector<Line>* getShadowLinePtr() const { return &m_shadowLines; }
    BossSpawner* getBossSpawnerPtr() { return &m_bossSpawner; };
    void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    typedef std::vector<std::vector<Tile>> Layer;

    std::vector<Layer> m_layers;
    std::vector<sf::Sprite> m_linearSprite;
    std::vector<Light*> m_lights;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::Vector2i m_dimensions;

    Layer m_hitboxData;
    std::vector<CustomHitbox> m_customHitboxes;

    std::vector<Line> m_exitShadowLines;
    std::vector<Terrain> m_exits;
    std::vector<Terrain> m_exitBlockades;

    std::vector<Line> m_shadowLines;
    std::vector<Terrain> m_terrain;

    std::vector<std::vector<Line>> m_breakableShadowLines;
    std::vector<BreakableTerrain> m_breakableTerrain;

    std::vector<Tilemap> m_tilemaps;

    bool m_drawCollision;
    bool m_bossInLevel;
    BossSpawner m_bossSpawner;

    bool importLevel(const LevelInfo* level);
    bool generateHitboxes(int id, ColliderKeys type);
    void generateShadowLines(const std::vector<Terrain>& terrain, std::vector<Line>* vec);
    void generateBreakableShadowLines();
    void createSpites();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void generateBossSpawner(const CustomHitbox& box);


};