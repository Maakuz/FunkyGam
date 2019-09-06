#pragma once
#include "SFML/Graphics.hpp"

//Treated as a vec3 by shader
struct Light
{
    sf::Glsl::Vec2 pos;
    float radius;

    Light(sf::Vector2f pos = sf::Vector2f(0, 0), float radius = 0)
    {
        this->pos = pos;
        this->radius = radius;
    }
};