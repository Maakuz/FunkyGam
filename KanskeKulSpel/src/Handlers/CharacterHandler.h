#pragma once
#include "Entities/Enemies/Enemy.h"
#include "Entities/Player.h"
#include "Misc/Line.h"
#include <vector>

class CharacterHandler : public sf::Drawable
{
public:
    CharacterHandler();
    ~CharacterHandler();

    void initialize(const std::vector<Line>* occluders);

    void spawnEnemies();
    void update(float dt, sf::Vector2f mousePos);
    void queueColliders();
    
    void setSpawnPoints(std::vector<sf::Vector2f> spawnPoints) { this->spawnPoints = spawnPoints; };
    const Player& getPlayer() const { return *this->player; };

    void drawCollision(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    const std::vector<Line>* occluders;
    std::vector<sf::Vector2f> spawnPoints;
    std::vector<Enemy*> enemies;
    Player* player;

    void updateEnemyLineOfSight();


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};