#pragma once
#include <vector>
#include "Entities/Entity.h"


//todo: optimize
class CollisionHandler
{
public:
    struct CollisionData
    {
        Entity* causer;
        CollisionBox* box;
    };

    CollisionHandler() {};
    ~CollisionHandler() {};

    void processQueue();

    static void queueCollider(CollisionData collider);

private:
    static std::vector<CollisionData> boxes;
};