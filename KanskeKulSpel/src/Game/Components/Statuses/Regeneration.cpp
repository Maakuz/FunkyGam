#include "Regeneration.h"

Regeneration::Regeneration(int duration):
    Status(duration, Statuses::regeneration)
{
}

void Regeneration::resolve(Entity* owner, float dt)
{
}
