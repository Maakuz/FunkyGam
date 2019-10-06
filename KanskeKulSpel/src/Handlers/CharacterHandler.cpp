#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"

CharacterHandler::CharacterHandler()
{
   
}

CharacterHandler::~CharacterHandler()
{
    delete player;

    for (MovingEntity* enemy : enemies)
        delete enemy;
}

void CharacterHandler::initialize()
{
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150);

    this->player = new Player(data, sf::Vector2f(0, 0));
    this->player->setAnimationData(data);
}

void CharacterHandler::spawnEnemies()
{
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150);

    enemies.push_back(new Grunt(data, spawnPoints[0] + sf::Vector2f(0, -50)));
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
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    for (MovingEntity* enemy : enemies)
        target.draw(*enemy, states);
}
