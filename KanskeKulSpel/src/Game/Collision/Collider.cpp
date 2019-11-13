#include "Collider.h"
#include "Game/Misc/Definitions.h"
#include "SFML/Graphics.hpp"
#include "Game/Misc/VectorFunctions.h"

Collider::AABB::AABB(sf::Vector2f pos, sf::Vector2f size)
{
    this->pos = pos;
    this->size = size;
}


Collider::Collider(AABB collisionBox, bool enabled)
:box(collisionBox),
up(collisionBox),
down(collisionBox),
left(collisionBox),
right(collisionBox)
{
    this->enabled = enabled;

    setSides();
}

Collider::Collider(sf::Vector2f pos, sf::Vector2f size)
    :Collider(Collider::AABB(pos, size))
{
}

Collider::~Collider()
{
}

bool Collider::intersects(const AABB & other) const
{
    return (this->box.max().x >= other.min().x && this->box.max().y >= other.min().y &&
        other.max().x >= this->box.min().x && other.max().y >= this->box.min().y);
}

bool Collider::intersects(const AABB & a, const AABB & b)
{
    return (a.max().x >= b.min().x && a.max().y >= b.min().y &&
        b.max().x >= a.min().x && b.max().y >= a.min().y);
}

bool Collider::intersects(const Collider& other) const
{
    return this->intersects(other.getAABB());
}

bool Collider::contains(const sf::Vector2f point) const
{
    return this->box.min().x < point.x && this->box.max().x > point.x &&
        this->box.min().y < point.y && this->box.max().y > point.y;
}

bool Collider::contains(const AABB& aabb, sf::Vector2f point)
{
    return aabb.min().x < point.x && aabb.max().x > point.x &&
        aabb.min().y < point.y && aabb.max().y > point.y;
}

void Collider::addComponent(ColliderKeys key)
{
    this->components.emplace(key);
}

bool Collider::hasComponent(ColliderKeys component) const
{
    if (this->components.count(component))
        return true;

    return false;
}

void Collider::setAABB(AABB box)
{
    this->box = box;

    setSides();
}

void Collider::setPosition(sf::Vector2f pos)
{
    this->setAABB(AABB(pos, box.size));
}

void Collider::setSize(sf::Vector2f size)
{
    this->setAABB(AABB(box.pos, size));
}

sf::Vector2f Collider::calculateCollisionForceOnObject(sf::Vector2f objPos, sf::Vector2f colliderPos, sf::Vector2f objMomentum, sf::Vector2f colliderMomentum, float objMass, float colliderMass)
{
    float v1 = length(objMomentum);
    float v2 = length(colliderMomentum);
    float v1new = ((objMass - colliderMass) / (objMass + colliderMass) * v1) + (2 * colliderMass / (objMass + colliderMass) * v2);
    sf::Vector2f dir = objPos - colliderPos;
    normalize(dir);
    dir.x *= v1new;
    dir.y *= v1new;

    return dir;
}

void Collider::draw(sf::RenderTarget& target, sf::RenderStates states) const
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

void Collider::setSides()
{
    this->left = box;
    this->right = box;
    this->up = box;
    this->down = box;

    up.size.y = EDGE_SIZE;

    down.size.y = EDGE_SIZE;
    down.pos.y = box.max().y - down.size.y;

    right.size.x = EDGE_SIZE;
    right.size.y -= EDGE_SIZE;

    right.pos.x = box.max().x - right.size.x;
    right.pos.y += EDGE_SIZE * 0.5f;

    left.size.x = EDGE_SIZE;
    left.size.y -= EDGE_SIZE;

    left.pos.y += EDGE_SIZE * 0.5f;
}