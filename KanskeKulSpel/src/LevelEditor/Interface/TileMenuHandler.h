#pragma once
#include "LayerManager.h"
#include "LevelEditor/Interactables/Button.h"
#include "LevelEditor/Interactables\BoxyBox.h"
#include "LevelEditor/FileManager.h"
#include "Windows/SaveWindow.h"
#include "Windows/LoadWindow.h"
#include "LightManager.h"
#include <vector>

class TileMenuHandler
{
public:
    TileMenuHandler();
    virtual ~TileMenuHandler() {};

    void handleEvent(sf::Event event, bool guiActive, sf::Vector2i viewPortMousePos);
    void update(sf::Vector2i mousePos, sf::Vector2i viewPortMousePos, bool guiActive);

    void queueItems(sf::View viewArea);

    std::vector<ActiveTile> getActiveTile() { return activeTiles; };
    int getActiveTexture() { return activeTileTexture; };

    bool isRenderingLights() const { return renderingLights; };
    const sf::VertexArray& getGrid() const { return grid; };
    bool isGridVisible() const { return gridVisible; };
    void autosave();

private:
    void handleMouseEvents(sf::Event event, bool guiBlock, sf::Vector2i viewPortMousePos);
    void handleKeyboardEvents(sf::Event event);

    void createTileButtons();
    void handleMenuBar();
    void handleHelpWindow();
    void handleBlockSelection(sf::Vector2i start, sf::Vector2i stop);
    void handleLayerSelection(sf::Vector2i start, sf::Vector2i stop);
    void createActiveBounds(std::vector<ActiveTile>& tiles);
    void swapStartAndStopPosition(sf::Vector2i & start, sf::Vector2i & stop);
    void setActiveTexture(sf::String name, sf::String path);
    void importTexture(std::string name, std::string path);
    void equipEraser();
    bool anyWindowsOpen();

    void saveFile();
    void loadFile();

    void generateGrid(sf::Color color);


    std::vector<CustomHitbox> hitbaxes;

    FileManager fileManager;
    LayerManager layerManager;
    SaveWindow saveWindow;
    LoadWindow loadWindow;
    LightManager lightManager;
    BoxyBox tileBox;
    int activeTileTexture;
    std::vector<ActiveTile> activeTiles;
    std::vector<ActiveTile> copiedTiles;
    
    sf::Vector2i offset;

    sf::Vector2i pressedPos;
    sf::Vector2i releasedPos;

    std::vector<Button> buttons;

    sf::VertexArray grid;

    bool selectingBlocks;
    bool rightClicking;
    bool erasing;
    bool isImportingTexture;
    bool renderingLights;
    bool gridVisible;
    bool turboHitboxOverride;
};
