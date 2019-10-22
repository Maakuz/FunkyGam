#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game.h"
#include "Imgui/imgui.h"
#include "Imgui/SFML-imgui/imgui-SFML.h"
#include "Misc/Profiler.h"
#include "Misc/ConsoleWindow.h"
#include "Renderer.h"
#include "Handlers/TextureHandler.h"

int main()
{
#ifdef _DEBUG
    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
    _CrtSetDbgFlag(flag);
    //_CrtSetBreakAlloc(689); // Comment or un-comment on need basis
#endif

    bool printscreen = false;
    ConsoleWindow::get().addCommand("printScreen", [&](Arguments args)->std::string 
        {
            printscreen = true;
            return "screenshot saved";
        });

    srand((int)time(0));

    
    sf::RenderWindow wandow(sf::VideoMode(1920, 1080), "hehe");
    sf::Cursor curse;
    if (curse.loadFromSystem(sf::Cursor::Arrow))
        wandow.setMouseCursor(curse);
    
    wandow.setFramerateLimit(120);
    
    sf::Clock deltaTimer;
    sf::Time deltaTime;
    bool ProfilerActive = false;

    ImGui::SFML::Init(wandow);

    TextureHandler::get().loadTextures();
    Game game(&wandow);
    Renderer renderer(&wandow);

    while (wandow.isOpen())
    {
        sf::Event event;
        while (wandow.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                wandow.close();

            ImGui::SFML::ProcessEvent(event);

            // if (event.type == sf::Event::KeyPressed)
              //   printf("%d\n", event.key.code);

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
                ProfilerActive = !ProfilerActive;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            wandow.close();

        deltaTime = deltaTimer.restart();

        ImGui::SFML::Update(wandow, deltaTime);

        PROFILER_START("Update");
        game.update((float)deltaTime.asMilliseconds());
        PROFILER_STOP;

        PROFILER_START("Draw");
        wandow.clear(sf::Color(0, 100, 155));
        wandow.setView(game.getView());
        renderer.render(wandow);
        PROFILER_STOP;

        Profiler::get().updateProfiler(deltaTime.asMilliseconds(), &ProfilerActive);

        ImGui::SFML::Render(wandow);
        wandow.display();





        if (printscreen)
        {
            sf::RenderTexture tex;
            tex.create(WIN_WIDTH, WIN_HEIGHT);
            tex.clear(sf::Color::Blue);
            tex.setView(game.getView());
            renderer.render(tex);
            tex.display();
            
            tex.getTexture().copyToImage().saveToFile("test.png");

            printscreen = false;
        }
        //printf("%d\n", 1000 / deltaTime.asMilliseconds());
    }


    ImGui::SFML::Shutdown();

    return 0;
};


