#include "CollisionBox.h"

CollisionBox::CollisionBox(AABB collisionBox, bool enabled)
:box(collisionBox)
{
    this->enabled = enabled;
}

CollisionBox::CollisionBox(sf::Vector2f pos, sf::Vector2f size)
:box(pos, size)
{
    enabled = true;
}

bool CollisionBox::intersects(const AABB & other) const
{
    if (this->box.max().x < other.min().x || this->box.max().y < other.min().y ||
        other.max().x < this->box.min().x || other.max().y < this->box.min().y)
        return false;

    return true;
}

bool CollisionBox::intersects(const CollisionBox & other) const
{
    return this->intersects(other.getBox());
}

void CollisionBox::addComponent(collisionComponents comp)
{
    this->components.push_back(comp);
}