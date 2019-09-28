#include "Game.h"
#include "Misc/KeyboardState.h"
#include "Misc/Profiler.h"
#include "Handlers/TextureHandler.h"

#define DEBUG_MODE true

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


    this->view.setSize(sf::Vector2f(window->getSize()) / 2.f);

    this->view.setCenter(sf::Vector2f(window->getSize()) / 4.f);

    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150);

    this->player = new Player(data, sf::Vector2f(0, 0));
    this->player->setAnimationData(data);

    levelHandler.loadLevel();
}

Game::~Game()
{
    delete player;
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
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);
    
    KEYBOARD::KeyboardState::updateKeys();
    
    static Light light(player->getPosition(), 200, sf::Vector3f(1, 1, 1));
    light.pos = player->getPosition();
    LightQueue::get().queue(light);

    static Light light2(sf::Vector2f(0, 0), 2000, sf::Vector3f(1, 0.3, 0.3));
    LightQueue::get().queue(light2);

    static Light light3(sf::Vector2f(1000, 0), 2000, sf::Vector3f(0.5, 0.5, 0));
    LightQueue::get().queue(light3);
    
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::BackSpace))
        this->running = !this->running;

    PROFILER_START("projectileUpdate")
    this->projectileHandler.update(dt);
    PROFILER_STOP

    PROFILER_START("particleUpdate")
    this->particleHandler.update(dt);
    PROFILER_STOP
    
    PROFILER_START("PlayerUpdate")
    if (this->running)
        this->player->update(dt);
    PROFILER_STOP

    PROFILER_START("LevelUpdate")
    this->levelHandler.updateLevel(dt);
    PROFILER_STOP

    PROFILER_START("Collision")
    this->collisionHandler.queueCollider(this->player);
    this->collisionHandler.processQueue();
    PROFILER_STOP
    
}

void Game::draw()
{
    //Shadow map
    PROFILER_START("Shoaduv");
    this->renderTargets[0].clear(sf::Color::Transparent);
    this->shadowHandler.generateShadowMap(this->renderTargets[0]);
    this->renderTargets[0].display();

    this->fullscreenboi.setTexture(&this->renderTargets[0].getTexture());
    PROFILER_STOP

    PROFILER_START("Blur")
    for (int i = 0; i < 2; i++)
    {
        this->renderTargets[i + 1].clear(sf::Color::Transparent);
        this->renderTargets[i + 1].draw(this->fullscreenboi, &shaders[shaders.BLUR_PASS[i]]);
        this->renderTargets[i + 1].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i + 1].getTexture());
    }
    PROFILER_STOP

    LightQueue::get().clear();

#if DEBUG_MODE
    static int swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F1))
        swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F2))
        swap = 1;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F3))
        swap = 2;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F9))
        swap = 8;

    switch (swap)
    {
    case 0:
        break;

    case 1:
        this->fullscreenboi.setTexture(&renderTargets[0].getTexture());
        break;

    case 2:
        this->fullscreenboi.setTexture(&renderTargets[1].getTexture());
        break;

    case 8:
        
        break;

    default:
        break;
    }

    static bool drawGeometry = true;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F7))
        drawGeometry = !drawGeometry;

    if (drawGeometry)
        this->window->draw(this->levelHandler);


    this->window->draw(*player);
    this->window->draw(this->projectileHandler);
    this->window->draw(this->particleHandler);

    static bool skip = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F5))
        skip = !skip;

    if (!skip)
        this->window->draw(fullscreenboi, sf::BlendMultiply);

    else
        this->window->draw(fullscreenboi);

    static bool drawHitbaxes = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F6))
        drawHitbaxes = !drawHitbaxes;

    if (drawHitbaxes)
    {
        levelHandler.drawCollision(*window, sf::RenderStates::Default);
        window->draw(player->getCollisionBox());
    }
#else
    this->window->draw(this->levelHandler);
    this->window->draw(this->projectileHandler);
    this->window->draw(*player);
    this->window->draw(fullscreenboi);
#endif


}
