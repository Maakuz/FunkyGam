#pragma once

enum class ComponentKey
{
    flyingAI,
    groundAI,
    animatedSprite,
    collider,
    damage,
    health,
    movement,
    spell,
    sprite,
    tome,
    transform,
    logistic
};

class Comp
{
public:
    virtual ~Comp() {};
    virtual Comp* clone() const = 0;
};