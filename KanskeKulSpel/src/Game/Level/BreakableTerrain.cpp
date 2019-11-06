#include "BreakableTerrain.h"

BreakableTerrain::BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext)
    :Entity(pos, texture)
{
    overlayPresent = false;
    broken = false;
    collider.addComponent(Collider::ColliderKeys::ground);
    setTextureRect(texRext);
    addCollision();
    breakThreshold = 25;
}

void BreakableTerrain::handleCollision(const Entity* collider)
{
}

void BreakableTerrain::handleExplosion(const Explosion& explosion)
{
    if (explosion.calculateDamage(getCenterPos()) > this->breakThreshold)
        broken = true;
}

void BreakableTerrain::addOverlay(sf::Texture* texture, sf::IntRect texRext)
{
    overlayPresent = true;
    overlay.setTexture(*texture);
    overlay.setTextureRect(texRext);
    overlay.setPosition(this->getPosition());
}

void BreakableTerrain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    Entity::draw(target, states);
    if (overlayPresent)
        target.draw(overlay, states);
}
