#include "Game.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/Profiler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/ConsoleWindow.h"

#define DEBUG_MODE true

const float ZOOM_LEVEL = 2.f;

Game::Game(sf::RenderWindow* window)
{
    this->window = window;
    this->running = true;

    this->loadFiles();

    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);

    for (int i = 0; i < NR_OF_RENDER_TARGETS; i++)
    {
        this->renderTargets[i].create(window->getSize().x, window->getSize().y);
    }


    this->view.setSize(sf::Vector2f(window->getSize()) / ZOOM_LEVEL);

    levelHandler.loadLevel();
    charHandler.initialize(levelHandler.getShadowLinePtr());
    charHandler.setSpawnPoints(levelHandler.generateSpawnPoints());
    charHandler.spawnEnemies();
}

Game::~Game()
{
}

void Game::loadFiles()
{  

    TextureHandler::get().loadTextures();
    particleHandler.loadEmitters();

    this->shaders[SHADER::lighting].setUniform("shadowMap", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussVertical  ].setUniform("texture", sf::Shader::CurrentTexture);

}

void Game::update(float dt)
{
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window) / ZOOM_LEVEL;
    mousePos.x += this->view.getCenter().x - (view.getSize().x / ZOOM_LEVEL);
    mousePos.y += this->view.getCenter().y - (view.getSize().y / ZOOM_LEVEL);
    
    KEYBOARD::KeyboardState::updateKeys();
    MOUSE::MouseState::updateButtons();
    
    static Light light(this->charHandler.getPlayer().getPosition() + sf::Vector2f(32, 30), 200, sf::Vector3f(0.5f, 0.5f, 0.1f));
    light.pos = this->charHandler.getPlayer().getPosition();
    LightQueue::get().queue(&light);

    static Light light2(sf::Vector2f(0, 0), 2000, sf::Vector3f(0.5f, 0.05f, 0.05f));
    LightQueue::get().queue(&light2);

    static Light light3(sf::Vector2f(1000, 0), 2000, sf::Vector3f(0.1f, 0.1f, 0.f));
    LightQueue::get().queue(&light3);
    
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::BackSpace))
        this->running = !this->running;

    PROFILER_START("projectileUpdate");
    this->projectileHandler.update(dt);
    PROFILER_STOP;

    PROFILER_START("particleUpdate");
    this->particleHandler.update(dt);
    PROFILER_STOP;
    
    PROFILER_START("CharUpdate");
    if (this->running)
        this->charHandler.update(dt, mousePos);
    PROFILER_STOP;

    PROFILER_START("LevelUpdate");
    this->levelHandler.updateLevel(dt);
    PROFILER_STOP;

    PROFILER_START("Collision");
    this->projectileHandler.queueColliders();
    this->charHandler.queueColliders();
    this->collisionHandler.processQueue();
    PROFILER_STOP;

    sf::Vector2f center = this->charHandler.getPlayer().getPosition();
    center.x = std::max(center.x, view.getSize().x / ZOOM_LEVEL);
    center.x = std::min(center.x, levelHandler.getDimensions().x - (view.getSize().x / ZOOM_LEVEL));
    center.y = std::max(center.y, view.getSize().y / ZOOM_LEVEL);
    center.y = std::min(center.y, levelHandler.getDimensions().y - (view.getSize().y / ZOOM_LEVEL));
    this->view.setCenter(center);
    this->window->setView(this->view);
    
#if DEBUG_MODE true
    static bool debugActive = false;
    static bool debugActiveP = false;
    debugActiveP = debugActive;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(53))) //Tilde in sweden
        debugActive = !debugActive;

    static bool ProfilerActive = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::P)) //Tilde in sweden
        ProfilerActive = !ProfilerActive;

    if (debugActive)
    {
        PROFILER_START("Console");
        ConsoleWindow::get().update(!debugActiveP);
        PROFILER_STOP;
    }

    if (ProfilerActive)
        Profiler::get().updateProfiler(dt);

#endif
}

void Game::draw()
{




    //Shadow map
    PROFILER_START("Shoaduv");
    this->renderTargets[0].clear(sf::Color::Transparent);
    this->shadowHandler.generateShadowMap(this->renderTargets[0]);
    this->renderTargets[0].display();

    this->fullscreenboi.setTexture(&this->renderTargets[0].getTexture());
    PROFILER_STOP;

    PROFILER_START("Blur");
    for (int i = 0; i < 2; i++)
    {
        this->renderTargets[i + 1].clear(sf::Color::Transparent);
        this->renderTargets[i + 1].draw(this->fullscreenboi, &shaders[shaders.BLUR_PASS[i]]);
        this->renderTargets[i + 1].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i + 1].getTexture());
    }
    PROFILER_STOP;

    LightQueue::get().clear();

#if DEBUG_MODE
    static int swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F1))
        swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F2))
        swap = 1;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F3))
        swap = 2;

    switch (swap)
    {
    case 1:
        this->fullscreenboi.setTexture(&renderTargets[0].getTexture());
        break;

    case 2:
        this->fullscreenboi.setTexture(&renderTargets[1].getTexture());
        break;
    }

    static bool drawGeometry = true;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F4))
        drawGeometry = !drawGeometry;

    if (drawGeometry)
        this->window->draw(this->levelHandler);


    this->window->draw(this->charHandler);
    this->window->draw(this->projectileHandler);
    this->window->draw(this->particleHandler);

    static bool skip = false;
    static bool turboSkip = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F11))
    {
        skip = true;
        turboSkip = false;
    }
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F10))
    {
        skip = false;
        turboSkip = true;
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F9))
    {
        skip = false;
        turboSkip = false;
    }
    if (!skip && !turboSkip)
        this->window->draw(fullscreenboi, sf::BlendMultiply);

    else if (turboSkip) {}

    else
        this->window->draw(fullscreenboi);

    static bool drawHitbaxes = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F6))
        drawHitbaxes = !drawHitbaxes;

    if (drawHitbaxes)
    {
        levelHandler.drawCollision(*window, sf::RenderStates::Default);
        charHandler.drawCollision(*window, sf::RenderStates::Default);
    }

    static bool drawSightLines = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F7))
        drawSightLines = !drawSightLines;

    if (drawSightLines)
        charHandler.drawSightLines(*window, sf::RenderStates::Default);
#else
    this->window->draw(this->levelHandler);
    this->window->draw(this->projectileHandler);
    this->window->draw(*player);
    this->window->draw(fullscreenboi);
#endif


}
