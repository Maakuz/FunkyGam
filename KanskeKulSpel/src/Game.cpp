#include "Game.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/Profiler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Renderer.h"

#define DEBUG_MODE true

const float ZOOM_LEVEL = 1.f;

Game::Game(sf::RenderWindow* window)
{
    this->window = window;
    this->paused = false;
    this->gameState = GameState::hub;

    this->view.setSize(sf::Vector2f(window->getSize()) / ZOOM_LEVEL);
    particleHandler.loadEmitters();
    levelHandler.loadLevel();
    itemHandler.loadTemplates();
    uiHandler.initialize();
    charHandler.initialize(levelHandler.getShadowLinePtr(), &uiHandler);
    charHandler.setSpawnPoints(levelHandler.generateSpawnPoints());
    charHandler.spawnEnemies();
}

Game::~Game()
{
}

void Game::update(float dt)
{
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window) / ZOOM_LEVEL;
    mousePos.x += this->view.getCenter().x - (view.getSize().x / 2);
    mousePos.y += this->view.getCenter().y - (view.getSize().y / 2);
    
    KEYBOARD::KeyboardState::updateKeys();
    MOUSE::MouseState::updateButtons();

    switch (gameState)
    {
    case GameState::hub:
        updateHub(dt, mousePos);
        break;
    case GameState::level:
        updateLevel(dt, mousePos);
        break;
    default:
        break;
    }

#if DEBUG_MODE true
    static bool debugActive = false;
    static bool debugActiveP = false;
    debugActiveP = debugActive;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(53))) //Tilde in sweden
        debugActive = !debugActive;

    if (debugActive)
    {
        PROFILER_START("Console");
        ConsoleWindow::get().update(!debugActiveP);
        PROFILER_STOP;
    }
#endif
}

void Game::updateHub(float dt, sf::Vector2f mousePos)
{
    static Light light(sf::Vector2f(1300, 500), 800, sf::Vector3f(0.9f, 0.7f, 0.2f));
    LightQueue::get().queue(&light);

    this->view.setCenter((sf::Vector2f)window->getSize() / 2.f);

    Renderer::queueUI(&this->hubHandler);
}

void Game::updateLevel(float dt, sf::Vector2f mousePos)
{
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Home))
        this->paused = !this->paused;

    static Light light(this->charHandler.getPlayer().getPosition() + sf::Vector2f(32, 30), 400, sf::Vector3f(0.1f, 0.1f, 0.05f));
    light.pos = this->charHandler.getPlayer().getPosition();
    LightQueue::get().queue(&light);

    sf::Vector2f center = this->charHandler.getPlayer().getPosition();
    center.x = std::max(center.x, view.getSize().x / 2);
    center.x = std::min(center.x, levelHandler.getDimensions().x - (view.getSize().x / 2));
    center.y = std::max(center.y, view.getSize().y / 2);
    center.y = std::min(center.y, levelHandler.getDimensions().y - (view.getSize().y / 2));
    this->view.setCenter(center);

    PROFILER_START("projectileUpdate");
    this->itemHandler.update(dt);
    PROFILER_STOP;

    PROFILER_START("particleUpdate");
    this->particleHandler.update(dt);
    PROFILER_STOP;

    PROFILER_START("CharUpdate");
    if (!this->paused)
        this->charHandler.update(dt, mousePos);
    PROFILER_STOP;

    PROFILER_START("LevelUpdate");
    this->levelHandler.updateLevel(dt);
    PROFILER_STOP;

    PROFILER_START("Collision");
    this->itemHandler.queueColliders();
    this->charHandler.queueColliders();
    this->collisionHandler.processQueue();
    PROFILER_STOP;

    PROFILER_START("CalcLightLevel");
    charHandler.calculatePlayerIllumination();
    PROFILER_STOP;


    Renderer::queueDrawable(&this->levelHandler);
    Renderer::queueDrawable(&this->charHandler);
    Renderer::queueDrawable(&this->itemHandler);
    Renderer::queueDrawable(&this->particleHandler);
    Renderer::queueUI(&this->uiHandler);
}

/*
void Game::draw(sf::RenderTarget& target)
{
    target.setView(this->view);
    this->fullscreenboi.setPosition(view.getCenter() - (view.getSize() / 2.f));
    sf::Vector2f offset = view.getCenter() - (view.getSize() / 2.f);
    //Shadow map
    PROFILER_START("Shoaduv");
    this->renderTargets[0].clear(sf::Color::Black);
    this->renderTargets[0].setView(view);
    this->shadowHandler.generateShadowMap(this->renderTargets[0], offset);
    this->renderTargets[0].display();

    this->fullscreenboi.setTexture(&this->renderTargets[0].getTexture());
    PROFILER_STOP;

    PROFILER_START("Blur");
    for (int i = 0; i < 2; i++)
    {
        this->renderTargets[i + 1].setView(view);
        this->renderTargets[i + 1].clear(sf::Color::Transparent);
        this->renderTargets[i + 1].draw(this->fullscreenboi, &shaders[shaders.BLUR_PASS[i]]);
        this->renderTargets[i + 1].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i + 1].getTexture());
    }
    PROFILER_STOP;

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
        target.draw(this->levelHandler);


    target.draw(this->charHandler);
    target.draw(this->itemHandler);
    target.draw(this->particleHandler);

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
        target.draw(fullscreenboi, sf::BlendMultiply);

    else if (turboSkip) {}

    else
        target.draw(fullscreenboi);

    PROFILER_START("No shadow lights render");
    renderTargets[0].clear(sf::Color::Black);
    for (size_t i = 0; i < LightQueueNoShadow::get().getQueue().size(); i++)
    {
        Light_NoShadow* light = LightQueueNoShadow::get().getQueue()[i];
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("pos", light->pos - offset);
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("radius", light->radius);
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("color", light->color);

        sf::RenderStates state;
        state.shader = &ShaderHandler::getShader(SHADER::lightingNoShadow);
        state.blendMode = sf::BlendAdd;
        

        sf::RectangleShape sprite(sf::Vector2f(light->radius * 2, light->radius * 2));
        sprite.setPosition(light->pos - (sf::Vector2f(sprite.getSize() / 2.f)));
        renderTargets[0].draw(sprite, state);
    }
    renderTargets[0].display();

    fullscreenboi.setTexture(&renderTargets[0].getTexture());

    for (int i = 0; i < 2; i++)
    {
        renderTargets[i + 1].clear(sf::Color::Transparent);
        renderTargets[i + 1].draw(fullscreenboi, &shaders[shaders.BLUR_PASS[i]]);
        renderTargets[i + 1].display();

        fullscreenboi.setTexture(&renderTargets[i + 1].getTexture());
    }

    target.draw(fullscreenboi, sf::BlendAdd);
    PROFILER_STOP;

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

    target.setView(uiHandler.getView());
    target.draw(uiHandler);
#endif


}
*/