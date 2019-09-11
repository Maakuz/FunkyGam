#include "Game.h"
#include "KeyboardState.h"

#define DEBUG_MODE true

Game::Game(sf::RenderWindow* window)
{
    this->window = window;

    this->loadFiles();

    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);
    
    this->shadowMap.create(window->getSize().x, window->getSize().y);

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

    this->shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussVertical  ].setUniform("texture", sf::Shader::CurrentTexture);

}

void Game::update(float dt)
{
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);

    KEYBOARD::KeyboardState::updateKeys();
    

    this->player->update(dt);
    this->levelHandler.updateLevel(dt);


    this->collisionHandler.queueCollider(this->player);
    this->collisionHandler.processQueue();
}

void Game::draw()
{
    //Shadow map
    this->shadowMap.clear(sf::Color::White);
    this->shadowMap.draw(this->levelHandler, &shaders[SHADER::shadowMap]);
    this->shadowMap.display();

    ////Light drawing hopefully

    int nrOfLights = LightQueue::get().getQueue().size();
    shaders[SHADER::lighting].setUniform("nrOfLights", nrOfLights);
    shaders[SHADER::lighting].setUniformArray("lights", (sf::Glsl::Vec3*)LightQueue::get().getQueue().data(), nrOfLights);
    shaders[SHADER::lighting].setUniform("shadowMap", this->shadowMap.getTexture());

    for (int i = 0; i < NR_OF_RENDER_TARGETS; i++)
    {
        this->renderTargets[i].clear(sf::Color::Transparent);
        this->renderTargets[i].draw(this->fullscreenboi, &shaders[shaders.lightingPass[i]]);
        this->renderTargets[i].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i].getTexture());
    }

    LightQueue::get().clear();

#if DEBUG_MODE
    static int swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F1))
        swap = 0;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F2))
        swap = 1;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F3))
        swap = 2;

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F4))
        swap = 3;

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

    case 3:
        this->fullscreenboi.setTexture(&shadowMap.getTexture());
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
        this->window->draw(fullscreenboi);
#else
    this->window->draw(this->levelHandler);
    this->window->draw(*player);
    this->window->draw(fullscreenboi);
#endif




}
