#pragma once
#include "SFML/Graphics.hpp"
#include "Game/Interface/TextBubble.h"
#include "Game/Interface/UIHandler.h"
#include "Game/Level/Level.h"
#include "Game/Interface/Recipe.h"

class HubHandler : public sf::Drawable
{
public:
    HubHandler(UIHandler* uiHandler);
    ~HubHandler() {};

    void update(float dt, sf::Vector2f mousePos);
    const LevelInfo* getActiveLevel()const;
    const LevelInfo* changeToNextLevel(int exitTaken);
    const LevelInfo* changeToLevel(int levelID);
    void reset();
    void reload();

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