#include "Game.h"
#include "KeyboardState.h"

#define DEBUG_MODE true

Game::Game(sf::RenderWindow* window)
{
    this->window = window;

    this->loadFiles();

    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);
    

    for (int i = 0; i < NR_OF_RENDER_TARGETS; i++)
    {
        this->renderTargets[i].create(window->getSize().x, window->getSize().y);
    }


    this->view.setSize(1280 / 2, 720 / 2);

    this->view.setCenter(1280 / 4, 720 -  720 / 4);
    //window->setView(view);

    Player::AnimationData data(&this->textures.playerSprite, sf::Vector2u(6, 1), 150);

    this->player = new Player(data, sf::Vector2f(0, -1600));
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

    if (!this->textures.playerSprite.loadFromFile(TEXTURE_PATH("cateSheet.png")))
        exit(-2);

    this->shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussVertical  ].setUniform("texture", sf::Shader::CurrentTexture);

}

void Game::update(float dt)
{
    //printf("%f\n", dt);
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);

    KEYBOARD::KeyboardState::updateKeys();
    
    static int i = 0;
    i++;
    i %= 300;
    Light testLight(sf::Vector2f(500, 500), i);

    LightQueue::get().queue(testLight);

    this->player->update(dt);
    this->levelHandler.updateLevel(dt);


    this->collisionHandler.queueCollider(this->player);
    this->collisionHandler.processQueue();
}

void Game::draw()
{

    ////Light drawing hopefully

    int nrOfLights = LightQueue::get().getQueue().size();
    shaders[SHADER::lighting].setUniform("nrOfLights", nrOfLights);
    shaders[SHADER::lighting].setUniformArray("lights", (sf::Glsl::Vec3*)LightQueue::get().getQueue().data(), nrOfLights);

    for (int i = 0; i < NR_OF_RENDER_TARGETS; i++)
    {
        this->renderTargets[i].clear(sf::Color::Transparent);
        this->renderTargets[i].draw(this->fullscreenboi, &shaders[i]);
        this->renderTargets[i].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i].getTexture());
    }

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
    case 0:
        break;

    case 1:
        this->fullscreenboi.setTexture(&renderTargets[0].getTexture());
        break;

    case 2:
        this->fullscreenboi.setTexture(&renderTargets[1].getTexture());
        break;

    default:
        break;
    }



#endif


    //Window drawing
    this->window->clear(sf::Color(0, 155, 200));

    this->window->draw(this->levelHandler);
    this->window->draw(*player);

#if DEBUG_MODE
    static bool skip = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F5))
        skip = !skip;

    if (!skip)
        this->window->draw(fullscreenboi);

    static bool drawHitbaxes = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F6))
        drawHitbaxes = !drawHitbaxes;

    if (drawHitbaxes)
    { 
        levelHandler.drawCollision(*window, sf::RenderStates::Default);
        std::vector<sf::RectangleShape> recs;
        sf::RectangleShape r1(sf::RectangleShape(player->getCollisionBox().getAABB().size));
        r1.setPosition(player->getCollisionBox().getAABB().pos);

        recs.push_back(r1);


        for (auto & r : recs)
        {
            r.setFillColor(sf::Color::Green);
            window->draw(r);
        }
    

    }
#else
    this->window->draw(fullscreenboi);
#endif

    this->window->display();


    LightQueue::get().clear();
}