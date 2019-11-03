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

private:
    void scrollScreen(float dt);

    const sf::RenderWindow* window;
    TileMenuHandler tileMenuHandler;
    TileStager tileStager;
    sf::View workView;
    float zoom;
    bool guiActive;
    sf::Vector2i workSpaceMousePos;
};