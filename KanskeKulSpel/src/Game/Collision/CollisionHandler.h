#pragma once
#include <vector>
#include "Game/Entities/Collidable.h"
#include "Game/Level/BreakableTerrain.h"


class CollisionHandler
{
public:
    CollisionHandler() {};
    ~CollisionHandler() {};

    void processQueue(); //todo: optimize with quadtree and whatnot

    static void queueCollider(Collidable* causer);
    static void queueStaticCollider(Collidable* causer);
    static void queueExplosion(Explosion explosion);
    static void queueBreakable(BreakableTerrain* terrain);

    static const std::vector<Collidable*>* getStaticColliders() { return &staticColliders; };
private:
    static std::vector<Collidable*> colliders;
    static std::vector<Collidable*> staticColliders;
    static std::vector<BreakableTerrain*> breakableTerrain;
    static std::vector<Explosion> explosions;
};