#pragma once
#include "SFML/Window.hpp"
#include "Grid.h"


class Game
{
public:
    Game(sf::RenderWindow* window);
    ~Game();

    void update(float dt);
    void draw();
private:

    sf::RenderWindow* window;


    Grid testGrid;
};