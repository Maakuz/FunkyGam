#pragma once
#include "Game/Entities/Enemies/Bosses/Boss.h"
#include "Game/Entities/Enemies/Enemy.h"
#include "Game/Entities/Player.h"
#include "Game/Misc/Line.h"
#include "Game/Misc/BossSpawner.h"
#include "Renderer/DebugDrawable.h"
#include "Game/Level/Level.h"
#include <vector>

class CharacterHandler : public sf::Drawable, public DebugDrawable
{
public:
    static const int ENEMY_TEMPLATE_COUNT = 2;
    static const std::string ENEMIES[ENEMY_TEMPLATE_COUNT];
    enum EnemyType
    {
        grunt = 0,
        bird = 1
    };

    static const int BOSS_TEMPLATE_COUNT = 1;
    static const std::string BOSSES[BOSS_TEMPLATE_COUNT];
    enum BossType
    {
        fishmonger = 0
    };

    CharacterHandler(UIHandler* uiHandler);
    virtual ~CharacterHandler();

    CharacterHandler(const CharacterHandler&) = delete;

    static bool isBossActive() { return CharacterHandler::s_bossActive; };

    void initializeLevel(const std::vector<Line>* occluders, sf::Vector2f playerSpawnPoint);
    void loadPlayer();
    void loadEnemies();
    void spawnEnemies(const LevelInfo* info);
    void update(float dt, sf::Vector2f mousePos);
    void queueColliders();
    void setBossSpawner(BossSpawner* bossSpawner);

    void setSpawnPoints(std::vector<sf::Vector2f> spawnPoints) { this->m_spawnPoints = spawnPoints; };
    Player* getPlayer() const { return this->m_player; };
    void calculatePlayerIllumination();

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    static bool s_bossActive;

    const std::vector<Line>* m_occluders;
    std::vector<sf::Vector2f> m_spawnPoints;
    std::vector<Enemy*> m_enemies;

    BossSpawner* m_bossSpawner;
    Boss* m_boss;
    Player* m_player;
    UIHandler* m_ui;

    bool m_drawHitboxes;
    bool m_drawSightlines;
   
    std::vector<Enemy*> m_enemyTemplates;
    std::vector<Boss*> m_bossTemplates;

    void spawnBoss(BossType bossType, sf::Vector2f pos);
    Enemy* spawnEnemy(int enemyType);

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
