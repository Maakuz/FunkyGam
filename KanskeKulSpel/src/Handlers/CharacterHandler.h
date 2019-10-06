#pragma once
#include "Entities/Enemies/Grunt.h"
#include "Entities/Player.h"
#include <vector>

class CharacterHandler : public sf::Drawable
{
public:
    CharacterHandler();
    ~CharacterHandler();

    void initialize();

    void spawnEnemies();
    void update(float dt, sf::Vector2f mousePos);
    void queueColliders();
    
    void setSpawnPoints(std::vector<sf::Vector2f> spawnPoints) { this->spawnPoints = spawnPoints; };
    const Player& getPlayer() const { return *this->player; };

    void drawCollision(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    std::vector<sf::Vector2f> spawnPoints;
    std::vector<MovingEntity*> enemies;
    Player* player;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};