#include "Terrain.h"

Terrain::Terrain(AABB aabb, ColliderKeys type, std::string colliderFlag)
    :Entity(aabb.pos),
    collider(aabb.size, aabb.pos)
{
    collider.addComponent(ColliderKeys::Static);
    collider.addComponent(type);
    collider.setFlag(colliderFlag);
}
