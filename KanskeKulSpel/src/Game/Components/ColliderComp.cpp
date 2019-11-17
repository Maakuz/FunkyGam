#include "ColliderComp.h"
#include "Game/Misc/Definitions.h"
#include "SFML/Graphics.hpp"
#include "Game/Misc/VectorFunctions.h"

ColliderComp::ColliderComp(sf::Vector2f size, const sf::Vector2f pos):
    box(AABB(pos, size)),
    upAABB(AABB(pos, size)),
    downAABB(AABB(pos, size)),
    rightAABB(AABB(pos, size)),
    leftAABB(AABB(pos, size))
{
    setSides();
    setPosition(pos);
}

void ColliderComp::addComponent(ColliderKeys key)
{
    this->components.emplace(key);
}

bool ColliderComp::hasComponent(ColliderKeys component) const
{
    if (this->components.count(component))
        return true;

    return false;
}


bool ColliderComp::intersects(const AABB& other) const
{
    return (this->box.max().x >= other.min().x && this->box.max().y >= other.min().y &&
        other.max().x >= this->box.min().x && other.max().y >= this->box.min().y);
}

bool ColliderComp::intersects(const AABB& a, const AABB& b)
{
    return (a.max().x >= b.min().x && a.max().y >= b.min().y &&
        b.max().x >= a.min().x && b.max().y >= a.min().y);
}

bool ColliderComp::intersects(const ColliderComp& other) const
{
    return this->intersects(other.box);
}

bool ColliderComp::contains(const sf::Vector2f point) const
{
    return this->box.min().x < point.x && this->box.max().x > point.x &&
        this->box.min().y < point.y && this->box.max().y > point.y;
}

bool ColliderComp::contains(const AABB& aabb, sf::Vector2f point)
{
    return aabb.min().x < point.x && aabb.max().x > point.x &&
        aabb.min().y < point.y && aabb.max().y > point.y;
}

void ColliderComp::setAABB(AABB box)
{
    this->box = box;

    setSides();
}

void ColliderComp::setPosition(sf::Vector2f pos)
{
    this->setAABB(AABB(pos, box.size));
}

sf::Vector2f ColliderComp::adjustSize()
{
    int diffX = EDGE_SIZE - box.size.x;
    int diffY = EDGE_SIZE - box.size.y;

    if (diffX > 0)
    {
        box.size.x += diffX;

        //setSpriteOffset(sf::Vector2f(diffX / 2, this->spriteOffset.y));
    }

    if (diffY > 0)
    {
        box.size.y += diffY;

        //setSpriteOffset(sf::Vector2f(this->spriteOffset.x, diffY / 2));
    }

    setSize(box.size);
    return sf::Vector2f(diffX, diffY);
}

void ColliderComp::setSize(sf::Vector2f size)
{
    this->setAABB(AABB(box.pos, size));
}

sf::Vector2f ColliderComp::getCenterPos() const 
{
    return this->box.pos + (this->box.size / 2.f);
}

void ColliderComp::setSides()
{
    this->leftAABB = box;
    this->rightAABB = box;
    this->upAABB = box;
    this->downAABB = box;

    upAABB.size.y = EDGE_SIZE;

    downAABB.size.y = EDGE_SIZE;
    downAABB.pos.y = box.max().y - downAABB.size.y;

    rightAABB.size.x = EDGE_SIZE;
    rightAABB.size.y -= EDGE_SIZE;

    rightAABB.pos.x = box.max().x - rightAABB.size.x;
    rightAABB.pos.y += EDGE_SIZE * 0.5f;

    leftAABB.size.x = EDGE_SIZE;
    leftAABB.size.y -= EDGE_SIZE;

    leftAABB.pos.y += EDGE_SIZE * 0.5f;
}

float ColliderComp::left() const
{
    return this->box.pos.x;
}

float ColliderComp::right() const
{
    return this->box.pos.x + box.size.x;
}

float ColliderComp::down() const
{
    return this->box.pos.y + box.size.y;
}

float ColliderComp::up() const
{
    return this->box.pos.y;
}

float ColliderComp::width() const
{
    return box.size.x;
}

float ColliderComp::height() const
{
    return box.size.y;
}

sf::Vector2f ColliderComp::calculateCollisionForceOnObject(sf::Vector2f objPos, sf::Vector2f colliderPos, sf::Vector2f objMomentum, sf::Vector2f colliderMomentum, float objMass, float colliderMass)
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

void ColliderComp::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape rect(box.size);
    rect.setFillColor(sf::Color::Green);
    rect.setPosition(box.pos);

    target.draw(rect);

    sf::RectangleShape sides[4];
    sides[0].setPosition(upAABB.pos);
    sides[0].setSize(upAABB.size);

    sides[1].setPosition(downAABB.pos);
    sides[1].setSize(downAABB.size);

    sides[2].setPosition(leftAABB.pos);
    sides[2].setSize(leftAABB.size);

    sides[3].setPosition(rightAABB.pos);
    sides[3].setSize(rightAABB.size);

    for (int j = 0; j < 4; j++)
    {
        sides[j].setFillColor(sf::Color(255, int(255 * (j / 4.f)), 0, 255));
        target.draw(sides[j]);
    }
}

