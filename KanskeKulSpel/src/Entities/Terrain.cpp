#include "Terrain.h"

Terrain::Terrain(CollisionBox::AABB aabb, CollisionBox::colliderComponents type)
    :Entity(sf::Vector2f(), &sf::Texture()) //Not cute
{
    collisionBox.addComponent(CollisionBox::colliderComponents::Static);
    collisionBox.addComponent(type);
    addCollision(aabb);
    setPosition(aabb.pos);
}
