#pragma once
#include <SFML/Graphics.hpp>
#include "Interface/TileMenuHandler.h"
#include "LevelEditor/TileStager.h"


class Editor
{
public:
    Editor(const sf::RenderWindow* window);
    virtual ~Editor() {};
    void loadLevel(std::string filename);
    void update(sf::Time deltaTime);
    void handleEvents(sf::Event e);
    const sf::View& getView() { return workView; };

    void pause() { this->paused = true; };
    void resume() { this->paused = false; };
private:

    const sf::RenderWindow* window;
    TileMenuHandler tileMenuHandler;
    TileStager tileStager;
    sf::View workView;
    float zoom;
    bool guiActive;
    bool paused;
    sf::Vector2i workSpaceMousePos;

    void scrollScreen(float dt);
};