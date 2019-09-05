#include "Game.h"

Game::Game(sf::RenderWindow* window)
   : testGrid(sf::Vector2i(20, 20), sf::Vector2f(600, 300))
{
    this->window = window;

    this->loadFiles();

    this->player.setGridPos(sf::Vector2i(5, 5));
    player.placeOnGrid(testGrid);

    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);
    

    this->testure.create(window->getSize().x, window->getSize().y);
    this->testure2.create(window->getSize().x, window->getSize().y);
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

    if (!this->oof.loadFromFile(SHADER_PATH("Lighting.frag"), sf::Shader::Type::Fragment))
    {
        system("pause");
        exit(-23);
    }
    
    std::vector<sf::Glsl::Vec2>lightPoints;

    lightPoints.push_back(sf::Glsl::Vec2(100, 100));
    lightPoints.push_back(sf::Glsl::Vec2(800, 400));
    lightPoints.push_back(sf::Glsl::Vec2(500, 300));
    lightPoints.push_back(sf::Glsl::Vec2(200, 500));
    lightPoints.push_back(sf::Glsl::Vec2(700, 800));

    oof.setUniform("testSize", (int)lightPoints.size());
    oof.setUniformArray("test", lightPoints.data(), lightPoints.size());

    if (!this->oof2.loadFromFile(SHADER_PATH("GaussianVert.frag"), sf::Shader::Type::Fragment))
    {
        system("pause");
        exit(-23);
    }
    oof2.setUniform("texture", sf::Shader::CurrentTexture);

    if (!this->oof3.loadFromFile(SHADER_PATH("GaussianHor.frag"), sf::Shader::Type::Fragment))
    {
        system("pause");
        exit(-23);
    }
    oof3.setUniform("texture", sf::Shader::CurrentTexture);


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
   
    //Light drawing hopefully
    this->testure.clear(sf::Color::Red);
    this->testure.draw(this->fullscreenboi, &oof);
    this->testure.display();
    

    this->fullscreenboi.setTexture(&testure.getTexture());

    this->testure2.clear(sf::Color::Red);
    this->testure2.draw(this->fullscreenboi, &oof2);
    this->testure2.display();

    this->testure.clear(sf::Color::Red);
    this->testure.draw(this->fullscreenboi, &oof);
    this->testure.display();


    //Window drawing
    this->window->clear(sf::Color(0, 200, 255));
    this->window->draw(this->testGrid);

    for (auto& e : this->enemoos)
        this->window->draw(e);

    this->window->draw(this->player);

    sf::Sprite testSprite(testure2.getTexture());

    this->window->draw(testSprite);

    this->window->display();
}
