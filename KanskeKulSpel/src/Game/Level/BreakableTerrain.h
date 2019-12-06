#pragma once
#include "Game/Entities/Collidable.h"
#include "Game/Components/SpriteComp.h"
#include "Game/Components/TransformComp.h"

class BreakableTerrain : public Collidable, public sf::Drawable
{
public:
    BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext);
    virtual ~BreakableTerrain();

    virtual void handleCollision(const Collidable* collider);
    virtual void handleExplosion(const Explosion& explosion);

    void addOverlay(sf::Texture* texture, sf::IntRect texRext);
    bool isBroken() const { return this->broken; };
private:
    SpriteComp sprite;
    SpriteComp spriteOverlay;
    
    bool broken;
    bool overlay;
    int breakThreshold;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};