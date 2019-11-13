#pragma once
#include <vector>
#include "Game/Entities/Entity.h"
#include "Game/Level/BreakableTerrain.h"


class CollisionHandler
{
public:
    CollisionHandler() {};
    ~CollisionHandler() {};

    void processQueue(); //todo: optimize with quadtree and whatnot

    static void queueCollider(Entity* causer);
    static void queueStaticCollider(Entity* causer);
    static void queueExplosion(Explosion explosion);
    static void queueBreakable(BreakableTerrain* terrain);

    static const std::vector<Entity*>* getStaticColliders() { return &staticColliders; };
private:
    static std::vector<Entity*> colliders;
    static std::vector<Entity*> staticColliders;
    static std::vector<BreakableTerrain*> breakableTerrain;
    static std::vector<Explosion> explosions;
};