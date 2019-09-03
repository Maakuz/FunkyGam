#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Grid.h"



int main()
{
    sf::RenderWindow test(sf::VideoMode(1280, 720), "hehe");

    sf::Vector2f startPos(600, 300);
    
    Grid roomba(sf::Vector2i(10, 10), sf::Vector2f(600, 300));
    

    while (test.isOpen())
    {
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            test.close();
    
    
        test.clear(sf::Color(0, 200, 255));
        test.draw(roomba);
        test.display();
    
    }


    return 0;
};


