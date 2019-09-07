#pragma once
#include "SFML/Window.hpp"
#include "Entities/Player.h"
#include "Entities/Tile.h"
#include "ShaderHandler.h"
#include "Lighting/LightQueue.h"
#include "CollisionHandler.h"

#define MAX_LIGHTS 200
#define NR_OF_RENDER_TARGETS 3

#define TEXTURE_FOLDER "../Sprites/"
#define TEXTURE_PATH(x) TEXTURE_FOLDER x



struct Textures
    {
        sf::Texture
            floorPiece,
            playerSprite;
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
    CollisionHandler collisionHandler;

    sf::RenderWindow* window;
    Textures textures;

    Player* player;
    Tile* ground;

    std::vector<Entity> enemoos;
    sf::View view;

    ShaderHandler shaders;

    sf::RenderTexture renderTargets[NR_OF_RENDER_TARGETS];
    sf::RectangleShape fullscreenboi;
};