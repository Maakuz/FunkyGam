#pragma once
#include "SFML/Graphics.hpp"
#include "Interface/TextBubble.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler();
    ~HubHandler() {};

private:
    sf::Sprite background;
    TextBubble button;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};