#include "CollisionBox.h"
#include "Game/Misc/Definitions.h"
#include "SFML/Graphics.hpp"
#define EDGE_SIZE TILE_SIZE / 4

CollisionBox::CollisionBox(AABB collisionBox, bool enabled)
:box(collisionBox),
up(collisionBox),
down(collisionBox),
left(collisionBox),
right(collisionBox)
{
    this->enabled = enabled;

    setSides();
}

CollisionBox::CollisionBox(sf::Vector2f pos, sf::Vector2f size)
    :CollisionBox(CollisionBox::AABB(pos, size))
{
}

CollisionBox::~CollisionBox()
{
}

bool CollisionBox::intersects(const AABB & other) const
{
    return (this->box.max().x >= other.min().x && this->box.max().y >= other.min().y &&
        other.max().x >= this->box.min().x && other.max().y >= this->box.min().y);
}

bool CollisionBox::intersects(const AABB & a, const AABB & b) const
{
    return (a.max().x >= b.min().x && a.max().y >= b.min().y &&
        b.max().x >= a.min().x && b.max().y >= a.min().y);
}

bool CollisionBox::intersects(const CollisionBox & other) const
{
    return this->intersects(other.getAABB());
}

void CollisionBox::addComponent(ColliderKeys key)
{
    this->components.emplace(key);
}

bool CollisionBox::hasComponent(ColliderKeys component) const
{
    if (this->components.count(component))
        return true;

    return false;
}

void CollisionBox::setAABB(AABB box)
{
    this->box = box;

    setSides();
}

void CollisionBox::setPosition(sf::Vector2f pos)
{
    this->setAABB(AABB(pos, box.size));
}

void CollisionBox::setSize(sf::Vector2f size)
{
    this->setAABB(AABB(box.pos, size));
}

void CollisionBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape rect(box.size);
    rect.setFillColor(sf::Color::Green);
    rect.setPosition(box.pos);

    target.draw(rect);

    sf::RectangleShape sides[4];
    sides[0].setPosition(up.pos);
    sides[0].setSize(up.size);

    sides[1].setPosition(down.pos);
    sides[1].setSize(down.size);

    sides[2].setPosition(left.pos);
    sides[2].setSize(left.size);

    sides[3].setPosition(right.pos);
    sides[3].setSize(right.size);

    for (int j = 0; j < 4; j++)
    {
        sides[j].setFillColor(sf::Color(255, int(255 * (j / 4.f)), 0, 255));
        target.draw(sides[j]);
    }
}

void CollisionBox::setSides()
{
    this->left = box;
    this->right = box;
    this->up = box;
    this->down = box;

    up.size.y = EDGE_SIZE;

    down.size.y = EDGE_SIZE;
    down.pos.y = box.max().y - down.size.y;

    right.size.x = EDGE_SIZE;
    right.size.y -= (EDGE_SIZE * 2);

    right.pos.x = box.max().x - right.size.x;
    right.pos.y += EDGE_SIZE;

    left.size.x = EDGE_SIZE;
    left.size.y -= (EDGE_SIZE * 2);

    left.pos.y += EDGE_SIZE;
}
