#include "BreakableTerrain.h"

BreakableTerrain::BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext)
    :Entity(pos, texture)
{
    overlayPresent = false;
    collisionBox.addComponent(CollisionBox::ColliderKeys::Static);
    collisionBox.addComponent(CollisionBox::ColliderKeys::ground);
    setTextureRect(texRext);
    addCollision();
}

void BreakableTerrain::handleCollision(const Entity* collider)
{
}

void BreakableTerrain::handleExplosion(const Explosion& explosion)
{
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
    target.draw(*this, states);
    if (overlayPresent)
        target.draw(overlay, states);
}
