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
    window->setView(view);

    this->player = Player(sf::Vector2f(1280 / 4, 720 - 720 / 4));

    Player::AnimationData data(&this->textures.playerSprite, sf::Vector2u(3, 1), 300);

    this->player.setAnimationData(data);


}

Game::~Game()
{
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

    this->player.update(dt);

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


    //Window drawing
    this->window->clear(sf::Color(0, 155, 200));

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

    sf::Sprite test(textures.floorPiece);
    test.setPosition(0, 720 - textures.floorPiece.getSize().y);

    this->window->draw(test);
    this->window->draw(player);

#if DEBUG_MODE
    static bool skip = false;
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::F5))
        skip = !skip;

    if (!skip)
        this->window->draw(fullscreenboi);
#else
    this->window->draw(fullscreenboi);
#endif

    this->window->display();


    LightQueue::get().clear();
}
