#include "ProjectileHandler.h"
std::vector<Throwable> ProjectileHandler::throwables;

ProjectileHandler::ProjectileHandler()
{
}

void ProjectileHandler::update(float dt)
{
    for (auto & throwable : throwables)
        throwable.update(dt);
}

void ProjectileHandler::addThrowable(Throwable throwable)
{
    throwables.push_back(throwable);
}
