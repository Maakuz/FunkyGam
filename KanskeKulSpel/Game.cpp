#include "Game.h"

Game::Game(sf::RenderWindow* window)
   : testGrid(sf::Vector2i(10, 10), sf::Vector2f(600, 300))
{
    this->window = window;
    
}

Game::~Game()
{
}

void Game::update(float dt)
{
    printf("%f\n", dt);



}

void Game::draw()
{
    this->window->clear(sf::Color(0, 200, 255));
    this->window->draw(this->testGrid);
    this->window->display();
}
