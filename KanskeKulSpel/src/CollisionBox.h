#pragma once
#include "SFML/System/Vector2.hpp"
#include <vector>


class CollisionBox
{
public:
    enum collisionComponents
    {
        Ground = 0,
        Player = 1
    };

    struct AABB
    {
        sf::Vector2f min;
        sf::Vector2f max;
        
        AABB(sf::Vector2f min, sf::Vector2f max)
        {
            this->min = min;
            this->max = max;
        }
    };


    CollisionBox(AABB collisionBox);
    CollisionBox(sf::Vector2f min, sf::Vector2f max);
    ~CollisionBox();

    bool intersects(const AABB& other) const;

    AABB getBox() { return this->box; };
    void setBox(AABB box) { this->box = box; };

private:
    AABB box;
    std::vector<collisionComponents> components;
};