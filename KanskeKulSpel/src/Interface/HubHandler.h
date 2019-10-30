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
    sf::Texture* textureOn;
    sf::Texture* textureOff;
    sf::Texture* textureTextWindow;
    sf::Sprite background;
    
    std::vector<TextBubble> mainButtons;
    TextBubble backButton;
    
    Level levelSelected;
    UIHandler* ui;

    TextBubble recipeListBackground;
    TextBubble recipeDescBackground;
    int infoTextCharacterSize;
    sf::Color textFillColor;
    sf::Color textOutlineColor;

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
        void setFillColor(sf::Color color)
        {
            name.setFillColor(color);
            description.setFillColor(color);
        }

        void setOutlineColor(sf::Color color)
        {
            name.setOutlineColor(color);
            description.setOutlineColor(color);
        }

        void setCharacterSize(int size)
        {
            name.setCharacterSize(size);
            description.setCharacterSize(size);
        }
    };
    
    std::vector<Recipe> recipes;
    int selectedRecipe;

    void loadRecipes();
    void loadInterface();

    void updateBack(sf::Vector2f mousePos, State backstate);
    void updateMain(sf::Vector2f mousePos);
    void updateAlchemy(sf::Vector2f mousePos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};