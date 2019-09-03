#include "SFML/Window.hpp"



int main()
{
    sf::Window test(sf::VideoMode(100,100), "hehe");

    while (test.isOpen())
    {



        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            test.close();
    }


    return 0;
};


