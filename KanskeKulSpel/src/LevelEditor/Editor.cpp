#include "Editor.h"
#include "Imgui/SFML-imgui/imgui-SFML.h"
#include "Imgui/imgui.h"
#include "LevelEditor/Constants.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Renderer/Renderer.h"

#define SCROLL_SPEED 2

Editor::Editor(const sf::RenderWindow* window) :
    tileMenuHandler()
{
    this->window = window;
    this->workView = sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT));
    this->guiActive = false;
    this->paused = false;
    this->zoom = 1;
}

void Editor::loadLevel(std::string filename)
{
    tileMenuHandler.load(filename);
}

void Editor::update(sf::Time deltaTime)
{
    float dt = deltaTime.asMilliseconds();
    static long long autoSaveCounter = 0;
    autoSaveCounter += dt;
    if (autoSaveCounter > 100000)
    {
        autoSaveCounter = 0;
        tileMenuHandler.autosave();
    }
    
    tileStager.clearAllQueues();

    this->workView.setSize(WIN_WIDTH * zoom, WIN_HEIGHT * zoom);

    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    workSpaceMousePos = (sf::Vector2i)window->mapPixelToCoords(mousePos);
    guiActive = ImGui::IsAnyItemHovered() || ImGui::IsAnyWindowHovered();

    if (!paused)
    {
        scrollScreen(dt);
        tileMenuHandler.update(mousePos, workSpaceMousePos, guiActive);
    }

    tileMenuHandler.queueItems(this->workView);
    tileStager.stageTiles(tileMenuHandler.isOverlayInToolbox());

    if (tileMenuHandler.isGridVisible())
        Renderer::queueDrawable(&tileMenuHandler.getGrid());

    Renderer::setDrawLights(tileMenuHandler.isRenderingLights());
}

void Editor::handleEvents(sf::Event e)
{
    if (!paused)
    {
        if (e.type == sf::Event::MouseWheelMoved)
            this->zoom -= (e.mouseWheelScroll.wheel / 10.f);

        if (zoom < 1)
            zoom = 1;

        tileMenuHandler.handleEvent(e, guiActive, workSpaceMousePos);
    }
}

void Editor::scrollScreen(float dt)
{
    float scrollSpeed = SCROLL_SPEED * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        this->workView.move(scrollSpeed, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        this->workView.move(-scrollSpeed, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        this->workView.move(0, scrollSpeed);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        this->workView.move(0, -scrollSpeed);

    //prefer not to speak or do this
    if (this->workView.getCenter().x - this->workView.getSize().x / 2 - TILEMENU_WIDTH + (TILEMENU_WIDTH * zoom) < 0)
        this->workView.setCenter(((this->workView.getSize().x / 2) + TILEMENU_WIDTH) - (TILEMENU_WIDTH * zoom), this->workView.getCenter().y);

    if (this->workView.getCenter().y - this->workView.getSize().y / 2 - MENU_BAR_HEIGHT + (MENU_BAR_HEIGHT * zoom) < 0)
        this->workView.setCenter(this->workView.getCenter().x, this->workView.getSize().y / 2 + MENU_BAR_HEIGHT - (MENU_BAR_HEIGHT * zoom));
}
