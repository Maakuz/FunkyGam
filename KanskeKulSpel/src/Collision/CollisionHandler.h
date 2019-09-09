#pragma once
#include <vector>
#include "Entities/Entity.h"


class CollisionHandler
{
public:
    CollisionHandler() {};
    ~CollisionHandler() {};

    void processQueue(); //todo: optimize with quadtree and whatnot

    static void queueCollider(Entity* causer);

private:
    static std::vector<Entity*> colliders;
};