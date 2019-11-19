#include "FishMonger.h"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset):
    Enemy(data, pos,size, offset)
{
}

void FishMonger::update(float dt)
{
}

void FishMonger::handleCollision(const Collidable* collidable)
{
}

void FishMonger::handleExplosion(const Explosion& explosion)
{
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}
