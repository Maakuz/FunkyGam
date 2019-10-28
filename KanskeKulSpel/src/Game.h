#pragma once
#include "SFML/Window.hpp"
#include "Handlers/CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/LevelHandler.h"
#include "Handlers/ItemHandler.h"
#include "Handlers/ParticleHandler.h"
#include "Interface/UIHandler.h"
#include "Interface/HubHandler.h"

#define MAX_LIGHTS 200
#define NR_OF_RENDER_TARGETS 3

enum class GameState 
{
    hub,
    level
};

class Game
{
public:
    Game(sf::RenderWindow* window);
    ~Game();

    void update(float dt);
    const sf::View& getView() { return view; };
    
private:
    void updateHub(float dt);
    void updateLevel(float dt);
    void loadLevel(int level);

    sf::Vector2f mousePos;
    sf::Vector2f mousePosWorld;

    bool paused;
    GameState gameState;

    CollisionHandler collisionHandler;
    LevelHandler levelHandler;
    ItemHandler itemHandler;
    ParticleHandler particleHandler;
    CharacterHandler charHandler;
    UIHandler uiHandler;
    HubHandler hubHandler;
    
    sf::RenderWindow* window;

    sf::View view;
};