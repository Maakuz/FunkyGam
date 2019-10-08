#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"
#include "Entities/Enemies/Grunt.h"

CharacterHandler::CharacterHandler()
{
   
}

CharacterHandler::~CharacterHandler()
{
    delete player;

    for (MovingEntity* enemy : enemies)
        delete enemy;
}

void CharacterHandler::initialize(const std::vector<Line>* occluders)
{
    this->occluders = occluders;
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150, sf::Vector2u(1, 0));
    this->player = new Player(data, sf::Vector2f(0, 0));
    this->player->setAnimationData(data);
}

void CharacterHandler::spawnEnemies()
{
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150, sf::Vector2u(1, 0));
    Grunt* grunt = new Grunt(data, spawnPoints[0] + sf::Vector2f(0, -50));
    grunt->setColor(sf::Color(255, 50, 50, 255));
    enemies.push_back(grunt);
}

void CharacterHandler::update(float dt, sf::Vector2f mousePos)
{
    this->player->update(dt, mousePos);
    
    for (MovingEntity* enemy : enemies)
    {
        Grunt* g = dynamic_cast<Grunt*>(enemy);
        if (g)
            g->update(dt);
    }
}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(this->player);

    for (MovingEntity* enemy : enemies)
        CollisionHandler::queueCollider(enemy);
}

void CharacterHandler::drawCollision(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(player->getCollisionBox(), states);

    for (MovingEntity* enemy : enemies)
        target.draw(enemy->getCollisionBox(), states);
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    for (MovingEntity* enemy : enemies)
        target.draw(*enemy, states);
}
