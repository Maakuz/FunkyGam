#pragma once
#include "Game/Entities/Collidable.h"

class Hazard : public Collidable
{
public:
    Hazard(sf::Vector2f pos, sf::Vector2f size, int damage = 0, DamageComp::DamageOrigin origin = DamageComp::DamageOrigin::neutral, bool projectilesCanPass = true) :
        Collidable(pos, size) 
    {
        DamageComp* damageComp = new DamageComp(origin);
        damageComp->damage = damage;
        addComponent<DamageComp>(damageComp);

        switch (origin)
        {
        case DamageComp::DamageOrigin::player:
            getColliderComp()->addComponent(ColliderKeys::player);
            break;
        case DamageComp::DamageOrigin::enemies:
            getColliderComp()->addComponent(ColliderKeys::enemy);
            break;

        default:
            break;
        }

        getColliderComp()->addComponent(ColliderKeys::hazard);

        if (projectilesCanPass)
            getColliderComp()->addComponent(ColliderKeys::projectilePassable);
    };
    
    virtual ~Hazard() {};

    virtual void handleCollision(const Collidable* collidable) {};
    virtual void handleExplosion(const Explosion& explosion) {};
    void setPos(sf::Vector2f pos) { getColliderComp()->setPosition(pos); };

private:
};