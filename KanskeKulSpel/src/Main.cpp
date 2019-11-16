#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game/Game.h"
#include "Imgui/imgui.h"
#include "Imgui/SFML-imgui/imgui-SFML.h"
#include "Misc/ConsoleWindow.h"
#include "Misc/Profiler.h"
#include "Renderer/Renderer.h"
#include "Game/Handlers/TextureHandler.h"
#include "LevelEditor/Editor.h"

enum class State 
{
    game,
    editor
};

int main()
{
#ifdef _DEBUG
    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
    _CrtSetDbgFlag(flag);
    //_CrtSetBreakAlloc(689); // Comment or un-comment on need basis
#endif
    sf::Color clearColor(0, 100, 155);

    bool printscreen = false;

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
    Editor editor(&wandow);

    State state = State::game;


    ConsoleWindow::get().addCommand("printScreen", [&](Arguments args)->std::string
        {
            printscreen = true;
            return "screenshot saved";
        });

    ConsoleWindow::get().addCommand("setClearColor", [&](Arguments args)->std::string
        {
            if (args.size() < 3)
                return "Missing argument int r, int g, int b";

            int r = std::min(std::stoi(args[0]), 255);
            int g = std::min(std::stoi(args[1]), 255);
            int b = std::min(std::stoi(args[2]), 255);

            if (r < 0)
                r = clearColor.r;

            if (g < 0)
                g = clearColor.g;

            if (b < 0)
                b = clearColor.b;

            sf::Color col(r, g, b);

            clearColor = col;
            return "Clear color is now " + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b);
        });

    ConsoleWindow::get().addCommand("setFrameCap", [&](Arguments args)->std::string
        {
            if (args.size() < 1)
                return "Missing argument int framerate";

            int framerate = std::stoi(args[0]);
            
            wandow.setFramerateLimit(framerate);

            return "Framerate is now " + std::to_string(framerate);
        });

    ConsoleWindow::get().addCommand("editLevel", [&](Arguments args)->std::string
        {
            state = State::editor;
            if (game.getCurrentLevel())
                editor.loadLevel(game.getCurrentLevel()->levelFileName);
            return "Editiong";
        });

    ConsoleWindow::get().addCommand("quitEditor", [&](Arguments args)->std::string
        {
            state = State::game;
            game.resetAfterEditing();
            return "Thanks for tiling";
        });

    bool debugActive = false;
    bool debugActivePrev = false;
    while (wandow.isOpen())
    {
        sf::Event e;
        while (wandow.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                wandow.close();

            ImGui::SFML::ProcessEvent(e);

            // if (event.type == sf::Event::KeyPressed)
              //   printf("%d\n", event.key.code);

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Key(53)) //Tilde in sweden
                debugActive = !debugActive;

            if (!ImGui::IsAnyItemActive() && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::P) //P in sweden
                ProfilerActive = !ProfilerActive;


            if (e.type == sf::Event::LostFocus)
            {
                game.pause();
                editor.pause();
            }

            if (e.type == sf::Event::GainedFocus)
            {
                game.resume();
                editor.resume();
            }

            if (state == State::editor)
                editor.handleEvents(e);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            wandow.close();

        deltaTime = deltaTimer.restart();

        ImGui::SFML::Update(wandow, deltaTime);

        if (debugActive)
        {
            PROFILER_START("Console");
            ConsoleWindow::get().update(!debugActivePrev);
            PROFILER_STOP;
        }
        debugActivePrev = debugActive;

        PROFILER_START("Update");
        if (state == State::game)
        {
            game.update((float)deltaTime.asMilliseconds());
            wandow.setView(game.getView());
        }

        else
        {
            editor.update(deltaTime);
            wandow.setView(editor.getView());
        }
        PROFILER_STOP;

        PROFILER_START("Draw");
        wandow.clear(clearColor);
        renderer.render(wandow);
        renderer.renderDebug(wandow);
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


