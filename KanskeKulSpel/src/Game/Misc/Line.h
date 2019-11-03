#pragma once
#include "Definitions.h"
struct Line
{
    sf::Vector2f p1;
    sf::Vector2f p2;

    Line(sf::Vector2f p1 = sf::Vector2f(0, 0), sf::Vector2f p2 = sf::Vector2f(0, 0))
    {
        this->p1 = p1;
        this->p2 = p2;
    }
};

//if direction is not normalized ima be mad. Returns distance to intersection from pos
inline float findIntersectionPoint(sf::Vector2f pos, sf::Vector2f dir, sf::Vector2f p1, sf::Vector2f p2)
{
    sf::Vector2f v1 = pos - p1;
    sf::Vector2f v2 = p2 - p1;
    sf::Vector2f v3 = sf::Vector2f(-dir.y, dir.x);
    float dot = (v2.x * v3.x) + (v2.y * v3.y);

    float t1 = -1;
    float t2 = -1;

    if (abs(dot) > EPSYLONE)
    {
        //crioss
        t1 = ((v2.x * v1.y) - (v2.y * v1.x)) / dot;
        //dot
        t2 = ((v1.x * v3.x) + (v1.y * v3.y)) / dot;

        if (t1 < 0.f || (t2 + EPSYLONE < 0.f || t2 > 1.f + EPSYLONE))
        {
            t1 = -1;
        }
    }

    return t1;
}
