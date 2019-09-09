#include "Terrain.h"

Terrain::Terrain(CollisionBox::AABB aabb)
    :Entity(sf::Vector2f(), &sf::Texture()) //Not cute
{
    collisionBox.addComponent(CollisionBox::colliderComponents::Ground);
    collisionBox.setAABB(aabb);
    setPosition(aabb.pos);
}
