#include "CollisionHandler.h"
#include "Game/Misc/VectorFunctions.h"

std::vector<Collidable*> CollisionHandler::colliders;
std::vector<Collidable*> CollisionHandler::staticColliders;
std::vector<Explosion> CollisionHandler::explosions;
std::vector<BreakableTerrain*> CollisionHandler::breakableTerrain;

void CollisionHandler::processQueue()
{
    for (auto entity1 = colliders.begin(); entity1 < colliders.end(); entity1++)
    {
        for (auto entity2 = entity1 +1; entity2 < colliders.end(); entity2++)
        {
            if ((*entity1)->getComponent<ColliderComp>()->intersects(*(*entity2)->getComponent<ColliderComp>()))
            {
                (*entity1)->handleCollision((*entity2));
                (*entity2)->handleCollision((*entity1));
            }
        }
    }

    //Static objects cannot collide with other static objects
    for (const Collidable* staticEntity : staticColliders)
        for (Collidable* entity : colliders)
            if (staticEntity->getComponent<ColliderComp>()->intersects(*entity->getComponent<ColliderComp>()))
                entity->handleCollision(staticEntity);



    for (Explosion& explosion : explosions)
        for (Collidable* entity : colliders)
            if (lengthSquared(explosion.center - entity->getComponent<ColliderComp>()->getCenterPos()) < explosion.radius * explosion.radius)
                entity->handleExplosion(explosion);



    for (BreakableTerrain* terrain : breakableTerrain)
    {
        for (Explosion& explosion : explosions)
            if (lengthSquared(explosion.center - terrain->getColliderComp()->getCenterPos()) < explosion.radius * explosion.radius)
                terrain->handleExplosion(explosion);
    
        for (Collidable* entity : colliders)
        {
            if (terrain->getColliderComp()->intersects(*entity->getComponent<ColliderComp>()))
            {
                entity->handleCollision(terrain);
                terrain->handleCollision(entity);
            }
        }
    }

    CollisionHandler::breakableTerrain.clear();
    CollisionHandler::explosions.clear();
    CollisionHandler::colliders.clear();
    CollisionHandler::staticColliders.clear();
}

void CollisionHandler::queueCollider(Collidable* causer)
{
    CollisionHandler::colliders.push_back(causer);
}

void CollisionHandler::queueStaticCollider(Collidable* causer)
{
    CollisionHandler::staticColliders.push_back(causer);
}

void CollisionHandler::queueExplosion(Explosion explosion)
{
    CollisionHandler::explosions.push_back(explosion);
}

void CollisionHandler::queueBreakable(BreakableTerrain* terrain)
{
    CollisionHandler::breakableTerrain.push_back(terrain);
}
