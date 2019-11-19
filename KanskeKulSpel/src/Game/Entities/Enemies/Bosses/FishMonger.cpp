#include "FishMonger.h"

FishMonger::FishMonger(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset):
    Boss(data, pos,size, offset),
    groundMove(pos, size)
{
}

void FishMonger::update(float dt)
{


    Enemy::updateEnemy(dt);
}

void FishMonger::handleCollision(const Collidable* collidable)
{
    groundMove.handleCollision(collidable);
}

void FishMonger::handleExplosion(const Explosion& explosion)
{
}

std::istream& FishMonger::readSpecific(std::istream& in)
{
    return in;
}