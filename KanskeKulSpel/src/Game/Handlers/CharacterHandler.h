#pragma once
#include "Game/Entities/Enemies/Enemy.h"
#include "Game/Entities/Player.h"
#include "Game/Misc/Line.h"
#include "Renderer/DebugDrawable.h"
#include "Game/Level/Level.h"
#include <vector>

class CharacterHandler : public sf::Drawable, public DebugDrawable
{
public:
    static const int ENEMY_TEMPLATE_COUNT = 2;
    static const std::string ENEMIES[ENEMY_TEMPLATE_COUNT];
    enum enemy
    {
        grunt = 0,
        bird = 1
    };

    CharacterHandler(UIHandler* uiHandler);
    ~CharacterHandler();

    void initializeLevel(const std::vector<Line>* occluders, sf::Vector2f playerSpawnPoint);
    void loadPlayer();
    void loadEnemies();
    void spawnEnemies(const LevelInfo* info);
    void update(float dt, sf::Vector2f mousePos);
    void queueColliders();

    void setSpawnPoints(std::vector<sf::Vector2f> spawnPoints) { this->spawnPoints = spawnPoints; };
    Player* getPlayer() const { return this->player; };
    void calculatePlayerIllumination();

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    const std::vector<Line>* occluders;
    std::vector<sf::Vector2f> spawnPoints;
    std::vector<Enemy*> enemies;
    Player* player;
    UIHandler* ui;

    bool drawHitboxes;
    bool drawSightlines;
   

    std::vector<Enemy*> enemyTemplates;
    void updateEnemyLineOfSight(Enemy* enemy);

    template <typename EnemyType>
    EnemyType* createEnemy(AnimationData data, sf::Vector2f size, sf::Vector2f offset);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};

template<typename EnemyType> 
inline EnemyType* CharacterHandler::createEnemy(AnimationData data, sf::Vector2f size, sf::Vector2f offset)
{
    EnemyType* enemy = new EnemyType(data, sf::Vector2f(0, 0), size, offset);
    return enemy;
}
