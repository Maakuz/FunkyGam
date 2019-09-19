#include "CollisionHandler.h"

std::vector<Entity*> CollisionHandler::colliders;
std::vector<Entity*> CollisionHandler::staticColliders;

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
