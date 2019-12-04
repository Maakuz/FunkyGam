#include "Terrain.h"

Terrain::Terrain(AABB aabb, ColliderKeys type, std::string colliderFlag):
    Collidable(aabb.pos, aabb.size)
{
    ColliderComp* collider = getColliderComp();
    collider->addComponent(ColliderKeys::Static);
    collider->addComponent(type);
    collider->setFlag(colliderFlag);
}
