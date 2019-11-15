#pragma once
#include "SFML/Window.hpp"
#include "Handlers/CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "Level/LevelHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Item/ProjectileHandler.h"
#include "Particles/ParticleHandler.h"
#include "Interface/UIHandler.h"
#include "Interface/HubHandler.h"
#include "Game/Item/ItemEditor.h"
#include "Game/Item/RecipeEditor.h"
#include "Game/Particles/ParticleEditor.h"
#include "GameState.h"

#define MAX_LIGHTS 200
#define NR_OF_RENDER_TARGETS 3



class Game
{
public:
    Game(const sf::RenderWindow* window);
    ~Game();

    void update(float dt);
    const sf::View& getView() { return view; };
    const LevelInfo* getCurrentLevel()const { return hubHandler.getActiveLevel(); };
    void resetAfterEditing();
    void pause() { this->paused = true; };
    void resume() { this->paused = false; };

private:
    void updateHub(float dt);
    void updateLevel(float dt);
    void loadLevel(const LevelInfo* level);

    sf::Vector2f mousePos;
    sf::Vector2f mousePosWorld;

    bool paused;
    GameState gameState;

    CollisionHandler collisionHandler;
    LevelHandler levelHandler;
    ItemHandler itemHandler;
    ProjectileHandler projectileHandler;
    ParticleHandler particleHandler;
    CharacterHandler characterHandler;
    UIHandler uiHandler;
    HubHandler hubHandler;

    ItemEditor itemEditor;
    ParticleEditor particleEditor;
    RecipeEditor recipeEditor;

    const sf::RenderWindow* window;

    sf::View view;
};