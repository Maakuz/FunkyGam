#include "Tile.h"

Tile::Tile(sf::Vector2f pos, sf::Texture* texture)
    :Entity(pos, texture)
{
    this->collisionBox.addComponent(CollisionBox::colliderComponents::Ground);
}
