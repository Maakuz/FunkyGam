#pragma once
#include "SFML/Graphics.hpp"
#include "Interface/TextBubble.h"
#include "Interface/UIHandler.h"
#include "Level/Levels.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler(UIHandler* uiHandler);
    ~HubHandler() {};

    void update(float dt, sf::Vector2f mousePos);
    Level getLevelSelected()const { return levelSelected; };

    void reset();

private:
    enum class State
    {
        main,
        level,
        inventory,
        achemist
    };
    State state;
    static const int MAIN_BUTTON_COUNT = 3;
    sf::Sprite background;
    TextBubble mainButtons[MAIN_BUTTON_COUNT];
    TextBubble backButton;
    Level levelSelected;

    UIHandler* ui;

    void updateBack(sf::Vector2f mousePos, State backstate);
    void updateMain(sf::Vector2f mousePos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};