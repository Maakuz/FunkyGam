#include "CollisionHandler.h"
#include "Game/Misc/VectorFunctions.h"

std::vector<Entity*> CollisionHandler::colliders;
std::vector<Entity*> CollisionHandler::staticColliders;
std::vector<Explosion> CollisionHandler::explosions;
std::vector<BreakableTerrain*> CollisionHandler::breakableTerrain;

void CollisionHandler::processQueue()
{
    for (auto entity1 = colliders.begin(); entity1 < colliders.end(); entity1++)
    {
        for (auto entity2 = entity1 +1; entity2 < colliders.end(); entity2++)
        {
            if ((*entity1)->getCollisionBox().intersects((*entity2)->getCollisionBox()))
            {
                (*entity1)->handleCollision((*entity2));
                (*entity2)->handleCollision((*entity1));
            }
        }
    }

    //Static objects cannot collide with other static objects
    for (const Entity* staticEntity : staticColliders)
        for (Entity* entity : colliders)
            if (staticEntity->getCollisionBox().intersects(entity->getCollisionBox()))
                entity->handleCollision(staticEntity);



    for (Explosion& explosion : explosions)
        for (Entity* entity : colliders)
            if (lengthSquared(explosion.center - entity->getCenterPos()) < explosion.radius * explosion.radius)
                entity->handleExplosion(explosion);



    for (BreakableTerrain* terrain : breakableTerrain)
    {
        for (Explosion& explosion : explosions)
            if (lengthSquared(explosion.center - terrain->getCenterPos()) < explosion.radius * explosion.radius)
                terrain->handleExplosion(explosion);
    
        for (Entity* entity : colliders)
        {
            if (terrain->getCollisionBox().intersects(entity->getCollisionBox()))
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

void CollisionHandler::queueBreakable(BreakableTerrain* terrain)
{
    CollisionHandler::breakableTerrain.push_back(terrain);
}
