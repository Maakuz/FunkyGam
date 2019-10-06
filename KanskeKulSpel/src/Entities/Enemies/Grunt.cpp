#include "Grunt.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :MovingEntity(data, pos)
{
    
}

void Grunt::update(float dt)
{

    
    MovingEntity::update(dt);
}
