#include "CollisionHandler.h"
#include "Misc/VectorFunctions.h"

std::vector<Entity*> CollisionHandler::colliders;
std::vector<Entity*> CollisionHandler::staticColliders;
std::vector<Explosion> CollisionHandler::explosions;

void CollisionHandler::processQueue()
{
    for (int i = 0; i < colliders.size(); i++)
    {
        for (int j = i + 1; j < colliders.size(); j++)
        {
            if (colliders[i]->getCollisionBox().intersects(colliders[j]->getCollisionBox()))
            {
                colliders[i]->handleCollision(*colliders[j]);
                colliders[j]->handleCollision(*colliders[i]);
            }
        }
    }

    //Static objects cannot collide with other static objects
    for (int i = 0; i < staticColliders.size(); i++)
    {
        for (int j = 0; j < colliders.size(); j++)
        {
            if (staticColliders[i]->getCollisionBox().intersects(colliders[j]->getCollisionBox()))
            {
                colliders[j]->handleCollision(*staticColliders[i]);
            }
        }
    }

    for (int i = 0; i < explosions.size(); i++)
    {
        for (int j = 0; j < colliders.size(); j++)
        {
            if (lengthSquared(explosions[i].center - colliders[j]->getCenterPos()) < explosions[i].radius * explosions[i].radius)
                colliders[j]->handleExplosion(explosions[i]);
        }
    }

    CollisionHandler::explosions.clear();
    CollisionHandler::colliders.clear();
    CollisionHandler::staticColliders.clear();
}

void CollisionHandler::queueCollider(Entity* causer)
{
    CollisionHandler::colliders.push_back(causer);
}

void CollisionHandler::queueStaticCollider(Entity* causer)
{
    CollisionHandler::staticColliders.push_back(causer);
}

void CollisionHandler::queueExplosion(Explosion explosion)
{
    CollisionHandler::explosions.push_back(explosion);
}
