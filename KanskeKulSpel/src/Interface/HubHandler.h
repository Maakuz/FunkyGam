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
        alchemist
    };
    State state;
    static const int MAIN_BUTTON_COUNT = 3;
    sf::Sprite background;
    TextBubble mainButtons[MAIN_BUTTON_COUNT];
    TextBubble backButton;
    Level levelSelected;

    TextBubble recipeListBackground;

    UIHandler* ui;

    struct Recipe
    {
        sf::Text name;
        sf::Text description;
        std::vector<int> components;
        std::vector<int> componentAmounts;
        int resultID;
        bool unlocked;
        bool seen;

        Recipe();
    };
    
    std::vector<Recipe> recipes;
    void loadRecipes();

    void updateBack(sf::Vector2f mousePos, State backstate);
    void updateMain(sf::Vector2f mousePos);
    void updateAlchemy(sf::Vector2f mousePos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};