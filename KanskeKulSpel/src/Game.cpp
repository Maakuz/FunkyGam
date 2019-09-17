#include "Game.h"
#include "Misc/KeyboardState.h"
#include "Misc/Profiler.h"

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

    Player::AnimationData data(&this->textures.playerSprite, sf::Vector2u(6, 1), 150);

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

    
    if (!this->textures.floorPiece.loadFromFile(TEXTURE_PATH("floorPiece.png")))
        exit(-2);

    if (!this->textures.playerSprite.loadFromFile(TEXTURE_PATH("smallCate.png")))
        exit(-2);

    this->shaders[SHADER::lighting].setUniform("shadowMap", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussVertical  ].setUniform("texture", sf::Shader::CurrentTexture);

}

void Game::update(float dt)
{
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);

    KEYBOARD::KeyboardState::updateKeys();
    
    Light light(player->getPosition(), 500, sf::Vector3f(1, 1, 1));
    LightQueue::get().queue(light);
    
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::BackSpace))
        this->running = !this->running;

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
    


    ////Light drawing hopefully
    int nrOfLights = LightQueue::get().getQueue().size();
    shaders[SHADER::lighting].setUniform("pos", LightQueue::get().getQueue().at(0).pos);
    shaders[SHADER::lighting].setUniform("radius", LightQueue::get().getQueue().at(0).radius);
    shaders[SHADER::lighting].setUniform("color", LightQueue::get().getQueue().at(0).color);
   
    //Shadow map
    PROFILER_START("Shadow draw")
    this->renderTargets[0].clear(sf::Color::Transparent);
        this->shadowHandler.generateShadowMap(this->renderTargets[0], &shaders[SHADER::lighting]);
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



#endif


    //Window drawing



#if DEBUG_MODE
    static bool drawHitbaxes = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F6))
        drawHitbaxes = !drawHitbaxes;

    if (drawHitbaxes)
    {
        levelHandler.drawCollision(*window, sf::RenderStates::Default);
        window->draw(player->getCollisionBox());
    }

    static bool drawGeometry = true;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F7))
        drawGeometry = !drawGeometry;

    if (drawGeometry)
        this->window->draw(this->levelHandler);


    this->window->draw(*player);

    static bool skip = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F5))
        skip = !skip;

    if (!skip)
        this->window->draw(fullscreenboi, sf::BlendMultiply);
#else
    this->window->draw(this->levelHandler);
    this->window->draw(*player);
    this->window->draw(fullscreenboi);
#endif




}
