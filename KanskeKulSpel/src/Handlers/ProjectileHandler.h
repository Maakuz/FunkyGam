#pragma once
#include <vector>
#include "Entities/Throwable.h"

class ProjectileHandler
{
public:
    ProjectileHandler();
    ~ProjectileHandler() {};

    void update(float dt);

    static void addThrowable(Throwable throwable);

private:
    static std::vector<Throwable> throwables;
};