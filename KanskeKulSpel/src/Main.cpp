#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game.h"
#include "imgui.h"
#include "SFML-imgui/imgui-SFML.h"
#include "SFML-imgui/imgui-SFML_export.h"

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

    
    sf::RenderWindow wandow(sf::VideoMode(1920, 1080), "hehe");
    sf::Cursor curse;
    if (curse.loadFromSystem(sf::Cursor::Arrow))
        wandow.setMouseCursor(curse);
    
    wandow.setFramerateLimit(120);
    
    sf::Clock deltaTimer;
    sf::Time deltaTime;
    
    ImGui::SFML::Init(wandow);

    Game game(&wandow);
    

    while (wandow.isOpen())
    {
        sf::Event event;
        while (wandow.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                wandow.close();

            ImGui::SFML::ProcessEvent(event);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            wandow.close();

        deltaTime = deltaTimer.restart();

        ImGui::SFML::Update(wandow, deltaTime);
        game.update(deltaTime.asMilliseconds());

        wandow.clear(sf::Color(0, 155, 200));
        game.draw();
        ImGui::SFML::Render(wandow);
        wandow.display();

    }


    ImGui::SFML::Shutdown();

    return 0;
};


