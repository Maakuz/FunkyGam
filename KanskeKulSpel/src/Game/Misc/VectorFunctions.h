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

inline float dot(sf::Vector2f v1, sf::Vector2f v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

//Sets the rotation of the vector
inline sf::Vector2f rotateBy(float degrees, sf::Vector2f vec, const sf::Vector2f center = sf::Vector2f(0, 0))
{
    degrees *= 3.14159f / 180.f;
    const float cs = cos(degrees);
    const float sn = sin(degrees);

    vec.x -= center.x;
    vec.y -= center.y;

    float nx = float(vec.x * cs - vec.y * sn);
    float ny = float(vec.x * sn + vec.y * cs);

    vec.x = nx;
    vec.y = ny;

    vec.x += center.x;
    vec.y += center.y;
    return vec;
}