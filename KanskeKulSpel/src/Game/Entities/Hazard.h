#pragma once
#include "Game/Entities/Collidable.h"

class Hazard : public Collidable
{
public:
    int damage;
    sf::Vector2f knockbackDir;

    Hazard(sf::Vector2f pos, sf::Vector2f size, int damage = 0) :
        collider(size, pos) 
    {
        collider.addComponent(ColliderKeys::hazard);
        this->damage = damage;
    };
    
    virtual ~Hazard() {};

    virtual void handleCollision(const Collidable* collidable) {};
    virtual void handleExplosion(const Explosion& explosion) {};
    virtual const ColliderComp& getCollider()const { return this->collider; };

    void setPos(sf::Vector2f pos) { collider.setPosition(pos); };


private:
    ColliderComp collider;
};