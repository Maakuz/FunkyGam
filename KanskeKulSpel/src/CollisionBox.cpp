#include "CollisionBox.h"

CollisionBox::CollisionBox(AABB collisionBox, bool enabled)
:box(collisionBox)
{
    this->enabled = enabled;
}

CollisionBox::CollisionBox(sf::Vector2f min, sf::Vector2f max)
:box(min, max)
{
    enabled = true;
}

bool CollisionBox::intersects(const AABB & other) const
{
    if (box.max.x < other.min.x || box.max.y < other.min.y ||
        other.max.x < box.max.x || other.max.y < box.max.y)
        return false;

    return true;
}

void CollisionBox::addComponent(collisionComponents comp)
{
    this->components.push_back(comp);
}

void CollisionBox::setPosition(sf::Vector2f pos)
{
    this->box.max += pos;
    this->box.min += pos;
}
