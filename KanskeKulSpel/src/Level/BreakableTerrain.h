#pragma once
#include "Entities/Entity.h"

class BreakableTerrain : public Entity
{
public:
    BreakableTerrain(sf::Vector2f pos, sf::Texture* texture, sf::IntRect texRext);
    ~BreakableTerrain() {};

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion);

    void addOverlay(sf::Texture* texture, sf::IntRect texRext);
    bool isBroken() const { return this->broken; };
private:
    sf::Sprite overlay;
    bool overlayPresent;
    
    bool broken;
    int breakThreshold;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};