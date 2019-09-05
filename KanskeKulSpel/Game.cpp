#include "Game.h"
#include "KeyboardState.h"

#define DEBUG_MODE true

Game::Game(sf::RenderWindow* window)
   : testGrid(sf::Vector2i(20, 20), sf::Vector2f(600, 300))
{
    this->window = window;

    this->loadFiles();

    this->player.setGridPos(sf::Vector2i(5, 5));
    player.placeOnGrid(testGrid);

    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);
    

    for (int i = 0; i < RENDER_TARGET_AMOUNT; i++)
    {
        this->renderTargets[i].create(window->getSize().x, window->getSize().y);
    }
}

Game::~Game()
{
}

void Game::loadFiles()
{
    if (!textures.player.loadFromFile(TEXTURE_PATH("cate.png")))
        exit(-22);

    if (!textures.blob.loadFromFile(TEXTURE_PATH("blob.png")))
        exit(-22);
    
    pointLights.push_back(sf::Glsl::Vec2(100, 100));
    pointLights.push_back(sf::Glsl::Vec2(800, 400));
    pointLights.push_back(sf::Glsl::Vec2(500, 300));
    pointLights.push_back(sf::Glsl::Vec2(200, 500));
    pointLights.push_back(sf::Glsl::Vec2(700, 500));

    shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    shaders[SHADER::gaussVertical].setUniform("texture", sf::Shader::CurrentTexture);


    this->player.setTexture(textures.player);

    for (size_t i = 0; i < 10; i++)
    {
        enemoos.push_back(Entity(&textures.blob, sf::Vector2i(rand() % 10, rand() % 10)));
        enemoos[i].placeOnGrid(testGrid);
        printf("%d, %d\n", enemoos[i].getGridPos().x, enemoos[i].getGridPos().y);
    }

}

void Game::update(float dt)
{
    //printf("%f\n", dt);
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*this->window);

    KEYBOARD::KeyboardState::updateKeys();
    
    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::A))
        printf("Tut\n");

    sf::Vector2i gridlock;
    if (this->testGrid.isInsideGrid(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        this->testGrid.highlightTile(mousePos);
    }
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        this->testGrid.removeAllHighlights();
}

void Game::draw()
{

    ////Light drawing hopefully
    shaders[SHADER::lighting].setUniform("testSize", (int)pointLights.size());
    shaders[SHADER::lighting].setUniformArray("test", pointLights.data(), pointLights.size());

    for (int i = 0; i < RENDER_TARGET_AMOUNT; i++)
    {
        this->renderTargets[i].clear(sf::Color::Transparent);
        this->renderTargets[i].draw(this->fullscreenboi, &shaders[i]);
        this->renderTargets[i].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i].getTexture());
    }


    //Window drawing
    this->window->clear(sf::Color(0, 200, 255));
    this->window->draw(this->testGrid);

    for (auto& e : this->enemoos)
        this->window->draw(e);

    this->window->draw(this->player);

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



    this->window->draw(fullscreenboi);

    this->window->display();
}
