#include "BreakableTerrain.h"
#include "Game/Misc/Definitions.h"

BreakableTerrain::BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext):
    sprite(texture, pos),
    spriteOverlay(texture, pos),
    Collidable(pos, sf::Vector2f(TILE_SIZE, TILE_SIZE))
{
    this->broken = false;
    this->spriteOverlay = nullptr;


    getColliderComp()->addComponent(ColliderKeys::ground);
    sprite.setTextureRect(texRext);
    breakThreshold = 25;
    overlay = false;
}

BreakableTerrain::~BreakableTerrain()
{
}

void BreakableTerrain::handleCollision(const Collidable* collider)
{
}

void BreakableTerrain::handleExplosion(const Explosion& explosion)
{
    if (explosion.calculateDamage(getColliderComp()->getCenterPos()) > this->breakThreshold)
        broken = true;
}

void BreakableTerrain::addOverlay(sf::Texture* texture, sf::IntRect texRext)
{
    this->spriteOverlay = SpriteComp(texture, sprite.getPosition());
    this->spriteOverlay.setTextureRect(texRext);
    overlay = true;
}

void BreakableTerrain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);

    if (overlay)
        target.draw(spriteOverlay, states);
}
