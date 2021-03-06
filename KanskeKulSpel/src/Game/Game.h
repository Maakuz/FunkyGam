#pragma once
#include "SFML/Window.hpp"
#include "Handlers/CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "Level/LevelHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Particles/ParticleHandler.h"
#include "Interface/UIHandler.h"
#include "Interface/HubHandler.h"
#include "Game/Item/ItemEditor.h"
#include "Game/Item/RecipeEditor.h"
#include "Game/Particles/ParticleEditor.h"
#include "Game/Particles/TendrilTester.h"
#include "GameState.h"

class Game
{
public:
    Game(const sf::RenderWindow* window);
    virtual ~Game();

    void runAutoCommands();

    void update(float dt);
    const sf::View& getView() { return view; };
    const LevelInfo* getCurrentLevel()const { return hubHandler->getActiveLevel(); };
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

    CollisionHandler* collisionHandler;
    LevelHandler* levelHandler;
    UIHandler* uiHandler;
    ProjectileHandler* projectileHandler;
    ParticleHandler* particleHandler;
    ItemHandler* itemHandler;
    CharacterHandler* characterHandler;
    HubHandler* hubHandler;

    ItemEditor itemEditor;
    ParticleEditor particleEditor;
    RecipeEditor recipeEditor;
    TendrilTester tendrilTester;

    const sf::RenderWindow* window;

    sf::View view;
};