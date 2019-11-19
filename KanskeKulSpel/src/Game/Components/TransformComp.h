#pragma once
#include "SFML/System/Vector2.hpp"

class TransformComp
{
public:
    TransformComp(){};
    virtual ~TransformComp() {};

    sf::Vector2f pos;
};