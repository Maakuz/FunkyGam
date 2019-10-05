#pragma once
#include "SFML/System/Vector2.hpp"
#include <math.h>

inline float lengthSquared(sf::Vector2f vec)
{
    return abs(vec.x * vec.x) + abs(vec.y * vec.y);
}

inline float length(sf::Vector2f vec)
{
    return sqrt(lengthSquared(vec));
}

inline void normalize(sf::Vector2f& vec)
{
    float l = length(vec);
    vec.x = vec.x / l;
    vec.y = vec.y / l;
}