#include "Game.h"
#include "Misc/KeyboardState.h"
#include "Misc/MouseState.h"
#include "Misc/Profiler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Renderer/Renderer.h"

#define DEBUG_MODE true

const float ZOOM_LEVEL = 1.f;

Game::Game(const sf::RenderWindow* window) :
    hubHandler(&this->uiHandler),
    characterHandler(&this->uiHandler),
    itemHandler(&this->uiHandler)
{
    this->window = window;
    this->paused = false;
    this->gameState = GameState::States::hub;

    this->view.setSize(sf::Vector2f(window->getSize()) / ZOOM_LEVEL);
    particleHandler.loadEmitters();
    itemHandler.loadTemplates();
    uiHandler.initialize();
}

Game::~Game()
{
}

void Game::update(float dt)
{
    this->mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);
    this->mousePosWorld = this->mousePos / ZOOM_LEVEL;
    this->mousePosWorld.x += this->view.getCenter().x - (view.getSize().x / 2);
    this->mousePosWorld.y += this->view.getCenter().y - (view.getSize().y / 2);
    
    KEYBOARD::KeyboardState::updateKeys();
    MOUSE::MouseState::updateButtons();

    switch (gameState.getState())
    {
    case GameState::States::hub:
        updateHub(dt);
        break;
    case GameState::States::level:
        updateLevel(dt);
        break;
    default:
        break;
    }
}

void Game::resetAfterEditing()
{
    Renderer::setDrawLights(true);
    this->hubHandler.reload();
    if (this->gameState == GameState::States::level)
        loadLevel(this->hubHandler.getActiveLevel());
}

void Game::updateHub(float dt)
{
    static Light light(sf::Vector2f(1300, 500), 800, sf::Vector3f(0.9f, 0.7f, 0.2f));
    LightQueue::get().queue(&light);

    this->view.setCenter((sf::Vector2f)window->getSize() / 2.f);

    this->hubHandler.update(dt, this->mousePos);
    this->uiHandler.update(dt, this->mousePos);

    if (this->hubHandler.getActiveLevel() != nullptr)
    {
        loadLevel(this->hubHandler.getActiveLevel());
        this->gameState = GameState::States::level;
    }

    Renderer::queueUI(&this->hubHandler);
    Renderer::queueUI(&this->uiHandler);
}

void Game::updateLevel(float dt)
{
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Home))
        this->paused = !this->paused;

    static Light light(this->characterHandler.getPlayer()->getPosition() + sf::Vector2f(32, 30), 400, sf::Vector3f(0.1f, 0.1f, 0.05f));
    light.pos = this->characterHandler.getPlayer()->getPosition();
    LightQueue::get().queue(&light);

    sf::Vector2f center = this->characterHandler.getPlayer()->getPosition();
    center.x = std::max(center.x, view.getSize().x / 2);
    center.x = std::min(center.x, levelHandler.getDimensions().x - (view.getSize().x / 2));
    center.y = std::max(center.y, view.getSize().y / 2);
    center.y = std::min(center.y, levelHandler.getDimensions().y - (view.getSize().y / 2));
    this->view.setCenter(center);

    PROFILER_START("projectileUpdate");
    this->itemHandler.update(dt, characterHandler.getPlayer());
    PROFILER_STOP;

    PROFILER_START("particleUpdate");
    this->particleHandler.update(dt);
    PROFILER_STOP;

    PROFILER_START("CharUpdate");
    this->characterHandler.update(dt, mousePosWorld);
    if (!this->characterHandler.getPlayer()->isAlive() || 
        characterHandler.getPlayer()->isReturning())
    {
        this->gameState = GameState::States::hub;
        this->hubHandler.reset();
    }

    int goal = this->characterHandler.getPlayer()->getExitReached();
    if (goal != -1)
        this->loadLevel(hubHandler.changeToNextLevel(goal));

    PROFILER_STOP;

    PROFILER_START("UI_Update");
    this->uiHandler.update(dt, this->mousePos);
    PROFILER_STOP;

    PROFILER_START("LevelUpdate");
    this->levelHandler.updateLevel(dt);
    PROFILER_STOP;

    PROFILER_START("Collision");
    this->itemHandler.queueColliders();
    this->characterHandler.queueColliders();
    this->levelHandler.queueColliders();
    this->collisionHandler.processQueue();
    PROFILER_STOP;

    PROFILER_START("CalcLightLevel");
    characterHandler.calculatePlayerIllumination();
    PROFILER_STOP;


    Renderer::queueDrawable(&this->levelHandler);
    Renderer::queueDrawable(&this->characterHandler);
    Renderer::queueDrawable(&this->itemHandler);
    Renderer::queueDrawable(&this->particleHandler);
    Renderer::queueUI(&this->uiHandler);

    Renderer::queueDebug(&this->characterHandler);
    Renderer::queueDebug(&this->levelHandler);
}

void Game::loadLevel(const LevelInfo* level)
{
    LightQueue::get().clear();
    LightQueueNoShadow::get().clear();
    particleHandler.reset();
    levelHandler.loadLevel(level);
    characterHandler.initializeLevel(levelHandler.getShadowLinePtr(), levelHandler.findPlayerSpawnPoint());
    characterHandler.setSpawnPoints(levelHandler.generateEnemySpawnPoints());
    characterHandler.spawnEnemies();
    itemHandler.setGatherPoints(levelHandler.generateGatherPoints(), levelHandler.generateRareGatherPoints());
    itemHandler.spawnGatherables(level);
}