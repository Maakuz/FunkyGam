#include "Terrain.h"

Terrain::Terrain(CollisionBox::AABB aabb, CollisionBox::ColliderKeys type)
    :Entity(sf::Vector2f(), &sf::Texture()) //Not cute
{
    collisionBox.addComponent(CollisionBox::ColliderKeys::Static);
    collisionBox.addComponent(type);
    addCollision(aabb);
    setPosition(aabb.pos);
}
