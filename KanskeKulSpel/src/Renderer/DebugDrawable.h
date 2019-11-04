#pragma once
#include "SFML/Graphics/RenderTarget.hpp"
class DebugDrawable
{
public:
    DebugDrawable() {};
    ~DebugDrawable() {};

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const = 0;
    
private:

};