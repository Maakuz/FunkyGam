#pragma once
#include "SFML/Window.hpp"
#include "Handlers/ShaderHandler.h"
#include "Handlers/CharacterHandler.h"
#include "Handlers/ShadowHandler.h"
#include "Lighting/LightQueue.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/LevelHandler.h"
#include "Handlers/ProjectileHandler.h"
#include "Handlers/ParticleHandler.h"

#define MAX_LIGHTS 200
#define NR_OF_RENDER_TARGETS 3



class Game
{
public:
    Game(sf::RenderWindow* window);
    ~Game();

    void loadFiles();

    void update(float dt);
    void draw(sf::RenderTarget& target);
    
private:
    bool running;

    CollisionHandler collisionHandler;
    LevelHandler levelHandler;
    ProjectileHandler projectileHandler;
    ParticleHandler particleHandler;
    CharacterHandler charHandler;
    
    sf::RenderWindow* window;


    std::vector<Entity> enemoos;
    sf::View view;

    ShaderHandler shaders;
    ShadowHandler shadowHandler;


    sf::RenderTexture renderTargets[NR_OF_RENDER_TARGETS];
    sf::RectangleShape fullscreenboi;
};