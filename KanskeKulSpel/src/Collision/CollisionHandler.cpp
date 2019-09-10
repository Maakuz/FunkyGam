#include "CollisionHandler.h"

std::vector<Entity*> CollisionHandler::colliders;

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
                static int k = 0;
                k++;
            }
        }
    }


    CollisionHandler::colliders.clear();
}

void CollisionHandler::queueCollider(Entity* causer)
{
    CollisionHandler::colliders.push_back(causer);
}
