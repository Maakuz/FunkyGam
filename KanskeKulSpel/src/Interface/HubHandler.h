#pragma once
#include "SFML/Graphics.hpp"
#include "Interface/TextBubble.h"
#include "Interface/UIHandler.h"
#include "Level/Level.h"
#include "Interface/Info.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler(UIHandler* uiHandler);
    ~HubHandler() {};

    void update(float dt, sf::Vector2f mousePos);
    const LevelInfo* getActiveLevel()const;
    const LevelInfo* changeToNextLevel(int exitTaken);
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
    sf::Texture* textureInactive;
    sf::Texture* textureTextWindow;
    sf::Sprite background;

    TextBubble listBackground;
    TextBubble descriptionBox;
    TextBubble infoBox;
    
    std::vector<TextBubble> mainButtons;
    TextBubble backButton;
    
    int activeLevel;
    UIHandler* ui;

    sf::Sprite alchemist;
    sf::Sprite adventurer;
    TextBubble acceptButton;

    int infoTextCharacterSize;
    sf::Color textFillColor;
    sf::Color textOutlineColor;


    struct Recipe : public Info
    {
        std::vector<int> components;
        std::vector<int> componentAmounts;
        int resultID;
        int resultAmount;
        
        Recipe()
        {
            resultID = -1;
            resultAmount = 1;
        }
    };
    
    std::vector<Recipe> recipes;
    int selectedRecipe;

    std::vector<LevelInfo> levels;
    int selectedLevel;

    void loadRecipes();
    void loadLevelInfo();
    void loadInterface();

    void updateBack(sf::Vector2f mousePos, State backstate);
    void updateMain(sf::Vector2f mousePos);
    void updateLevelSelect(sf::Vector2f mousePos);
    void updateAlchemy(sf::Vector2f mousePos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};