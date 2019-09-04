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
    //printf("%f\n", dt);
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);
    sf::Vector2i gridlock;
    if (this->testGrid.isInsideGrid(mousePos))
    {
        gridlock = this->testGrid.getGridPos(mousePos);
        this->testGrid.getTile(gridlock).setFillColor(sf::Color::Black);
    }


}

void Game::draw()
{
    this->window->clear(sf::Color(0, 200, 255));
    this->window->draw(this->testGrid);
    this->window->display();
}
