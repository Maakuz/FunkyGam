#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game.h"

int main()
{
#ifdef _DEBUG
    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
    _CrtSetDbgFlag(flag);
    //_CrtSetBreakAlloc(689); // Comment or un-comment on need basis
#endif

    float x = 300, y = 400;
    float r = 300;



    srand((int)time(0));

    
    sf::RenderWindow wandow(sf::VideoMode(1280, 720), "hehe");
    sf::Cursor curse;
    if (curse.loadFromSystem(sf::Cursor::Arrow))
        wandow.setMouseCursor(curse);
    
    wandow.setFramerateLimit(120);
    
    sf::Clock deltaTimer;
    
    Game game(&wandow);
    

    while (wandow.isOpen())
    {
        game.update((float)deltaTimer.restart().asMilliseconds());
        
        game.draw();



        sf::Event event;
        while (wandow.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                wandow.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            wandow.close();
    }

    return 0;
};


