#pragma once
#include "Entities/Enemies/Enemy.h"
#include "Entities/Player.h"
#include "Misc/Line.h"
#include <vector>

class CharacterHandler : public sf::Drawable
{
public:
    static const int ENEMY_TEMPLATE_COUNT = 1;
    const std::string ENEMIES[ENEMY_TEMPLATE_COUNT] = { "grunt.mop" };
    enum enemy
    {
        grunt = 0
    };

    CharacterHandler();
    ~CharacterHandler();

    void initialize(const std::vector<Line>* occluders, UIHandler* uiHandler);
    void loadEnemies();
    void spawnEnemies();
    void update(float dt, sf::Vector2f mousePos);
    void queueColliders();

    void setSpawnPoints(std::vector<sf::Vector2f> spawnPoints) { this->spawnPoints = spawnPoints; };
    const Player& getPlayer() const { return *this->player; };
    void calculatePlayerIllumination(const sf::Texture* illuminationTexture);

    void drawCollision(sf::RenderTarget& target, sf::RenderStates states) const;
    void drawSightLines(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    const std::vector<Line>* occluders;
    std::vector<sf::Vector2f> spawnPoints;
    std::vector<Enemy*> enemies;
    Player* player;

    std::vector<Enemy*> enemyTemplates;
    void updateEnemyLineOfSight(Enemy* enemy);


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};