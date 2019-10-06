#pragma once
#include "../MovingEntity.h"

class Grunt : public MovingEntity
{
public:
    Grunt(AnimationData data, sf::Vector2f pos);
    ~Grunt() {};

    void update(float dt);

private:

};