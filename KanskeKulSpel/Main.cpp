#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game.h"



int main()
{
    sf::RenderWindow wandow(sf::VideoMode(1280, 720), "hehe");
    wandow.setFramerateLimit(120);
    
    sf::Clock deltaTimer;
    
    Game game(&wandow);
    

    while (wandow.isOpen())
    {
        
        game.update(deltaTimer.restart().asMilliseconds());

        game.draw();




        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            wandow.close();
    }


    return 0;
};


