#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(AnimationData data, sf::Vector2f pos, int health, int speed, int attack)
:Entity(pos, health, speed, attack)
{
    this->data = data;
}

void AnimatedEntity::update(float dt)
{
}
