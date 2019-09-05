#pragma once
#include "SFML/Window.hpp"
#include "Grid.h"
#include "Player.h"
#include "ShaderHandler.h"

#define MAX_LIGHTS 200
#define RENDER_TARGET_AMOUNT 3

#define TEXTURE_FOLDER "../Sprites/"
#define TEXTURE_PATH(x) TEXTURE_FOLDER x



struct Textures
    {
        sf::Texture
            player,
            blob;
    };


class Game
{
public:
    Game(sf::RenderWindow* window);
    ~Game();

    void loadFiles();

    void update(float dt);
    void draw();
    
private:

    sf::RenderWindow* window;
    Textures textures;

    Grid testGrid;

    Player player;
    std::vector<Entity> enemoos;

    ShaderHandler shaders;

    sf::RenderTexture renderTargets[RENDER_TARGET_AMOUNT];
    sf::RectangleShape fullscreenboi;
};