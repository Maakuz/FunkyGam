#include "BreakableTerrain.h"
#include "Game/Misc/Definitions.h"

BreakableTerrain::BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext):
    sprite(texture, pos),
    spriteOverlay(texture, pos),
    collider(sf::Vector2f(TILE_SIZE, TILE_SIZE), pos)
{
    this->broken = false;
    this->spriteOverlay = nullptr;
    this->collider.addComponent(ColliderKeys::ground);
    sprite.setTextureRect(texRext);
    breakThreshold = 25;
    overlay = false;
    transform.pos = pos;
}

BreakableTerrain::~BreakableTerrain()
{
}

void BreakableTerrain::handleCollision(const Collidable* collider)
{
}

void BreakableTerrain::handleExplosion(const Explosion& explosion)
{
    if (explosion.calculateDamage(collider.getCenterPos()) > this->breakThreshold)
        broken = true;
}

void BreakableTerrain::addOverlay(sf::Texture* texture, sf::IntRect texRext)
{
    this->spriteOverlay = SpriteComp(texture, transform.pos);
    this->spriteOverlay.setTextureRect(texRext);
    overlay = true;
}

void BreakableTerrain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);

    if (overlay)
        target.draw(spriteOverlay, states);
}
