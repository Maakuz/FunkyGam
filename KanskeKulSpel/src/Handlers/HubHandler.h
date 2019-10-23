#pragma once
#include "SFML/Graphics.hpp"
#include "Interface/TextBubble.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler();
    ~HubHandler() {};

    void update(float dt, sf::Vector2f mousePos);
    int getLevelSelected()const { return levelSelected; };

private:
    sf::Sprite background;
    TextBubble button;
    int levelSelected;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};