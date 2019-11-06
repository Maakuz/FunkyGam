#include "Terrain.h"

Terrain::Terrain(Collider::AABB aabb, Collider::ColliderKeys type, std::string colliderFlag)
    :Entity(sf::Vector2f(), &sf::Texture()) //Not cute
{
    collider.addComponent(Collider::ColliderKeys::Static);
    collider.addComponent(type);
    collider.setFlag(colliderFlag);
    addCollision(aabb);
    setPosition(aabb.pos);
}
