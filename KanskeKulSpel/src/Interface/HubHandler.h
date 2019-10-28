#pragma once
#include "SFML/Graphics.hpp"
#include "Interface/TextBubble.h"
#include "Interface/UIHandler.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler(UIHandler* uiHandler);
    ~HubHandler() {};

    void update(float dt, sf::Vector2f mousePos);
    int getLevelSelected()const { return levelSelected; };

    void reset();

private:
    enum class State
    {
        main,
        level,
        inventory
    };
    State state;
    static const int MAIN_BUTTON_COUNT = 2;
    sf::Sprite background;
    TextBubble mainButtons[MAIN_BUTTON_COUNT];
    TextBubble backButton;
    int levelSelected;

    UIHandler* ui;

    void updateMain(sf::Vector2f mousePos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};