#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"

CharacterHandler::CharacterHandler()
{
   
}

CharacterHandler::~CharacterHandler()
{
    delete player;
}

void CharacterHandler::initialize()
{
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150);

    this->player = new Player(data, sf::Vector2f(0, 0));
    this->player->setAnimationData(data);
}

void CharacterHandler::spawnEnemies()
{
    
}

void CharacterHandler::update(float dt, sf::Vector2f mousePos)
{
    this->player->update(dt, mousePos);

}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(this->player);
}

void CharacterHandler::drawCollision(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(player->getCollisionBox(), states);
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);
}
