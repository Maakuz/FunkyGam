#include "Window.h"
#include "Game/Handlers/TextureHandler.h"

Window::Window()
{
    m_clearColor = sf::Color(0, 100, 155);
    m_profilerActive = false;
    m_debugActive = false;
    m_debugActivePrev = false;
    m_focused = true;
    m_exitPromptActive = false;
    m_printScreen = false;

    srand((int)time(0));


    m_wandow.create(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "hehe");

    sf::Cursor curse;
    if (curse.loadFromSystem(sf::Cursor::Arrow))
        m_wandow.setMouseCursor(curse);

    m_wandow.setFramerateLimit(120);

    ImGui::SFML::Init(m_wandow);

    m_state = State::game;

    TextureHandler::get().loadTextures();

    m_game = new Game(&m_wandow);
    m_editor = new Editor(&m_wandow);
    m_renderer = new Renderer(&m_wandow);


    ConsoleWindow::get().addCommand("printScreen", [&](Arguments args)->std::string
        {
            m_printScreen = true;
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
                r = m_clearColor.r;

            if (g < 0)
                g = m_clearColor.g;

            if (b < 0)
                b = m_clearColor.b;

            sf::Color col(r, g, b);

            m_clearColor = col;
            return "Clear color is now " + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b);
        });

    ConsoleWindow::get().addCommand("setFrameCap", [&](Arguments args)->std::string
        {
            if (args.size() < 1)
                return "Missing argument int framerate";

            int framerate = std::stoi(args[0]);

            m_wandow.setFramerateLimit(framerate);

            return "Framerate is now " + std::to_string(framerate);
        });

    ConsoleWindow::get().addCommand("editLevel", [&](Arguments args)->std::string
        {
            m_state = State::editor;
            if (m_game->getCurrentLevel())
                m_editor->loadLevel(m_game->getCurrentLevel()->levelFileName);
            return "Editiong";
        });

    ConsoleWindow::get().addCommand("quitEditor", [&](Arguments args)->std::string
        {
            m_state = State::game;
            m_game->resetAfterEditing();
            return "Thanks for tiling";
        });

    m_game->runAutoCommands();
}

Window::~Window()
{
    delete m_game;
    delete m_editor;
    delete m_renderer;
}

int Window::run()
{
    while (m_wandow.isOpen())
    {

        handlePollEvents();

        if (m_focused && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            m_exitPromptActive = true;

        m_deltaTime = m_deltaTimer.restart();

        ImGui::SFML::Update(m_wandow, m_deltaTime);

        if (m_exitPromptActive)
        {
            pause();

            if (exitPrompt())
                m_wandow.close();

            if (!m_exitPromptActive)
            {
                resume();
            }
        }

        if (m_debugActive)
            ConsoleWindow::get().update(!m_debugActivePrev);

        m_debugActivePrev = m_debugActive;

        PROFILER_START("Update");
        if (m_state == State::game)
        {
            m_game->update((float)m_deltaTime.asMilliseconds());
            m_wandow.setView(m_game->getView());
        }

        else
        {
            m_editor->update(m_deltaTime);
            m_wandow.setView(m_editor->getView());
        }
        PROFILER_STOP;

        PROFILER_START("Draw");
        m_wandow.clear(m_clearColor);
        m_renderer->render(m_wandow);
        m_renderer->renderDebug(m_wandow);
        PROFILER_STOP;

        Profiler::get().updateProfiler(m_deltaTime.asMilliseconds(), &m_profilerActive);

        ImGui::SFML::Render(m_wandow);
        m_wandow.display();


        if (m_printScreen)
        {
            sf::RenderTexture tex;
            tex.create(WIN_WIDTH, WIN_HEIGHT);
            tex.clear(sf::Color::Blue);
            tex.setView(m_game->getView());
            m_renderer->render(tex);
            tex.display();

            tex.getTexture().copyToImage().saveToFile("test.png");

            m_printScreen = false;
        }
    }


    ImGui::SFML::Shutdown();

    return 0;
}

void Window::handlePollEvents()
{
    sf::Event e;
    while (m_wandow.pollEvent(e))
    {
        if (e.type == sf::Event::Closed)
            m_exitPromptActive = true;

        ImGui::SFML::ProcessEvent(e);

        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Key(53)) //Tilde in sweden
            m_debugActive = !m_debugActive;

        if (!ImGui::IsAnyItemActive() && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::P) //P in sweden
            m_profilerActive = !m_profilerActive;


        if (e.type == sf::Event::LostFocus)
        {
            pause();
            m_focused = false;
        }

        if (e.type == sf::Event::GainedFocus)
        {
            resume();
            m_focused = true;
        }

        if (m_state == State::editor)
            m_editor->handleEvents(e);
    }

}

void Window::takePrintScreen()
{
}

void Window::pause()
{
    m_game->pause();
    m_editor->pause();
}

void Window::resume()
{
    m_game->resume();
    m_editor->resume();
}

bool Window::exitPrompt()
{
    bool exiting = false;

    ImGui::Begin("Do you really want to quit? :(", &m_exitPromptActive);

    if (ImGui::Button("No!"))
        m_exitPromptActive = false;

    ImGui::SameLine();

    if (ImGui::Button("Yes!"))
        exiting = true;

    ImGui::End();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        exiting = true;

    return exiting;
}
