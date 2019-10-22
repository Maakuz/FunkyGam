#pragma once
#include "SFML/Graphics.hpp"

class HubHandler : public sf::Drawable
{
public:
    HubHandler();
    ~HubHandler() {};

private:
    sf::Sprite background;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};