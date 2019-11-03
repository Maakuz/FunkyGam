#pragma once
#include <vector>
#include "SFML/Graphics.hpp"
#include "Misc/Structs.h"
#include "LevelEditor/Interactables/ActiveTile.h"
#include <fstream>
#include "LevelEditor/Constants.h"
#define STATE_AMOUNTS 50


class LayerManager
{
public:
    LayerManager();
    virtual ~LayerManager() {};

    friend std::ostream & operator<<(std::ostream & out, const LayerManager & layerManager);
    friend std::istream & operator>>(std::istream & in, LayerManager & layerManager);

    void update(const std::vector<ActiveTile> & activeTiles, sf::Vector2i mousePos);
    void insertTiles(const std::vector<ActiveTile> & activeTiles, sf::Vector2i mousePos);
    
    void queueTiles(sf::View viewArea);

    std::vector<ActiveTile> getActiveTilesAt(sf::Vector2i start, sf::Vector2i stop);

    sf::Vector2i getWorkAreaStart() { return workAreaStart; };
    int getWorkAreaX() const { return layers[0][0].size(); };
    int getWorkAreaY() const { return layers[0].size(); };

    sf::Image getLayerAsImage(int layer) const;

    void setActiveLayer(int layer);
    int getActiveLayer() const { return this->activeLayer; };

    void setHighlightLayers(bool val);
    bool getHighlightLayers() const { return this->hightlightLayers; };

    //Clears all layers
    void startOver(int width = DEFAULT_WORK_AREA_X, int height = DEFAULT_WORK_AREA_Y);
    void resize(int width = DEFAULT_WORK_AREA_X, int height = DEFAULT_WORK_AREA_Y);

    void undo(int steps = 1);
    void redo(int steps = 1);

private:
    void processImagePart(sf::Image & image, int layer, int i, int j) const;

    void differentiateLayers();


    typedef std::vector<std::vector<Tile>> Layer;
    std::vector<Layer> layers;
    std::vector<std::vector<Layer>> prevStates;
    int currentState;

    int activeLayer;
    sf::Vector2i workAreaStart;

    bool hightlightLayers;
    bool showHitboxes;
    bool drawing;
};