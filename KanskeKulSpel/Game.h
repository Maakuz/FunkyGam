#pragma once
#include "SFML/Window.hpp"
#include "Grid.h"
#include "Player.h"

#define MAX_LIGHTS 200

#define SHADER_FOLDER "../Shaders/"
#define SHADER_PATH(x) SHADER_FOLDER x

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

    sf::Shader oof;
    sf::Shader oof2;
    sf::RenderTexture testure;
    sf::RenderTexture testure2;
    sf::RectangleShape fullscreenboi;
};