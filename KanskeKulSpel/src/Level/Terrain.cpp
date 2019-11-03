#include "Terrain.h"

Terrain::Terrain(CollisionBox::AABB aabb, CollisionBox::ColliderKeys type, std::string colliderFlag)
    :Entity(sf::Vector2f(), &sf::Texture()) //Not cute
{
    collisionBox.addComponent(CollisionBox::ColliderKeys::Static);
    collisionBox.addComponent(type);
    collisionBox.setFlag(colliderFlag);
    addCollision(aabb);
    setPosition(aabb.pos);
}
