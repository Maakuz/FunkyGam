#pragma once
#include "SFML/Graphics.hpp"

//Treated as a vec3 by shader
struct Light
{
    sf::Glsl::Vec2 pos;
    float radius;
    sf::Glsl::Vec3 color;
    sf::Glsl::Vec3 padding;

    Light(sf::Vector2f pos = sf::Vector2f(0, 0), float radius = 0, sf::Vector3f color = sf::Vector3f(1, 1, 1))
    {
        this->pos = pos;
        this->radius = radius;
        this->color = color;
        this->padding = sf::Vector3f(0, 0, 0);
    }
};