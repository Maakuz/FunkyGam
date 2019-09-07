#pragma once
#include "SFML/System/Vector2.hpp"
#include <set>


class CollisionBox
{
public:
    enum colliderComponents
    {
        Ground = 0,
        Player = 1
    };

    struct AABB
    {
        sf::Vector2f pos;
        sf::Vector2f size;
        
        AABB(sf::Vector2f pos, sf::Vector2f size)
        {
            this->pos = pos;
            this->size = size;
        }

        sf::Vector2f min() const
        {
            return this->pos;
        }

        sf::Vector2f max() const
        {
            return this->pos + this->size;
        }
    };


    CollisionBox(AABB collisionBox, bool enabled = true);
    CollisionBox(sf::Vector2f pos, sf::Vector2f size);
    ~CollisionBox() {};

    bool intersects(const AABB& other) const;
    bool intersects(const CollisionBox& other) const;

    void addComponent(colliderComponents comp);

    bool hasComponent(colliderComponents component) const;
    
    AABB getBox() const { return this->box; };
    void setBox(AABB box) { this->box = box; };
    void setPosition(sf::Vector2f pos) { this->box.pos = pos; };

    void enableCollision(bool boi) { this->enabled = boi; };
    bool isCollisionEnabled()const { return this->enabled; };

private:
    bool enabled;
    AABB box;
    std::set<colliderComponents> components;
};