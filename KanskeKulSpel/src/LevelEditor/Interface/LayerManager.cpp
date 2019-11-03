#include "LayerManager.h"
#include "LevelEditor/Constants.h"
#include "LevelEditor/Queues\TileQueue.h"
#include "LevelEditor/TileMaps.h"
#include "SFML\Window\Mouse.hpp"
#include <algorithm>

LayerManager::LayerManager()
{
    startOver();
    showHitboxes = false;
}

void LayerManager::update(const std::vector<ActiveTile> & activeTiles, sf::Vector2i mousePos)
{
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (drawing)
        {
            drawing = false;
           
            if (currentState != prevStates.size() - 1)
                prevStates.erase(prevStates.begin() + currentState + 1, prevStates.end());

            if (prevStates.size() >= STATE_AMOUNTS)
            {
                prevStates.erase(prevStates.begin());
                currentState--;
            }

            prevStates.push_back(layers);
            currentState++;

            //printf("State: %d\n", currentState);
            //printf("Size: %d\n", (int)prevStates.size());
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        insertTiles(activeTiles, mousePos);
    }
}

void LayerManager::insertTiles(const std::vector<ActiveTile>& activeTiles, sf::Vector2i mousePos)
{
    if (mousePos.x > workAreaStart.x &&
        mousePos.y > workAreaStart.y) // TODO: check if inside window
    {
        drawing = true;

        //Get what tile the mouse are over
        int x = (mousePos.x - workAreaStart.x) / TILE_SIZE;
        int y = (mousePos.y - workAreaStart.y) / TILE_SIZE;


        for (size_t i = 0; i < activeTiles.size(); i++)
        {
            int newX = x + activeTiles[i].x;
            int newY = y + activeTiles[i].y;

            if (newX < layers[0][0].size() && newY < layers[0].size())
            {
                layers[activeLayer][newY][newX].textureID = activeTiles[i].textureID;
                layers[activeLayer][newY][newX].tileID = activeTiles[i].tileID;
            }
        }
    }
}

//queue every tile
void LayerManager::queueTiles(sf::View viewArea)
{
    //Rounded up
    sf::Vector2i min(sf::Vector2i(viewArea.getCenter() - (viewArea.getSize() / 2.f)) / TILE_SIZE);
    sf::Vector2i max(min + (sf::Vector2i(viewArea.getSize()) / TILE_SIZE));
    min.x = std::max(min.x, 0);
    min.y = std::max(min.y, 0);
    
    max.y = std::min(max.y, (int)layers[0].size());
    max.x = std::min(max.x, (int)layers[0][0].size());

    for (size_t i = 0; i < TILE_LAYER_AMOUNT; i++)
    {
        for (int j = min.y; j < max.y; j++)
        {
            for (int k = min.x; k < max.x; k++)
            {
                if (layers[i][j][k].tileID != -1)
                    TileQueue::get().queue(layers[i][j][k]);
            }
        }
    }

    if (showHitboxes)
    {
        for (int j = min.y; j < max.y; j++)
        {
            for (int k = min.x; k < max.x; k++)
            {
                if (layers[LAYER_AMOUNT - 1][j][k].tileID != -1)
                {
                    layers[LAYER_AMOUNT - 1][j][k].color = sf::Color::Black;
                    TileQueue::get().queue(layers[LAYER_AMOUNT - 1][j][k]);
                }
            }
        }
    }
}

std::vector<ActiveTile> LayerManager::getActiveTilesAt(sf::Vector2i start, sf::Vector2i stop)
{
    std::vector<Tile> tiles;

    start -= workAreaStart;
    stop -= workAreaStart;

    start.x = std::max(start.x, 0);
    start.y = std::max(start.y, 0);
    stop.x = std::max(stop.x, 0);
    stop.y = std::max(stop.y, 0);

    start /= TILE_SIZE;
    stop  /= TILE_SIZE;

    int xCondition = std::min(stop.x, (int)layers[activeLayer][0].size() - 1);
    int yCondition = std::min(stop.y, (int)layers[activeLayer].size() - 1);

    for (size_t i = start.y; i <= yCondition; i++)
    {
        for (size_t j = start.x; j <= xCondition; j++)
        {
            //if (layers[activeLayer][i][j].tileID != -1) possible but confusing
            tiles.push_back(layers[activeLayer][i][j]);
        }
    }

    std::vector<ActiveTile> newActive;

    for (size_t i = 0; i < tiles.size(); i++)
    {
        ActiveTile active;
        active.tileID = tiles[i].tileID;
        active.textureID = tiles[i].textureID;
        active.x = (tiles[i].x - workAreaStart.x) / TILE_SIZE - start.x;
        active.y = (tiles[i].y - workAreaStart.y) / TILE_SIZE - start.y;

        active.box.setSize(TILE_SIZE, TILE_SIZE);
        active.box.setPosition(tiles[i].x, tiles[i].y);

        newActive.push_back(active);
    }

    return newActive;
}

sf::Image LayerManager::getLayerAsImage(int layer) const
{
    sf::Image image;
    int x = (int)layers[0][0].size();
    int y = (int)layers[0].size();
    image.create(x * TILE_SIZE, y * TILE_SIZE, sf::Color::Transparent);


    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            this->processImagePart(image, layer, i, j);
        }
    }

    return image;
}

void LayerManager::setActiveLayer(int layer) //This can potentially break if layer >= 3 TODO
{

    if (layer >= TILE_LAYER_AMOUNT)
    {
        setHighlightLayers(false);
        showHitboxes = true;
    }

    else
        showHitboxes = false;

    activeLayer = layer;

    differentiateLayers();
}

void LayerManager::setHighlightLayers(bool val)
{
    hightlightLayers = val;

    if (!hightlightLayers)
        for (int i = 0; i < TILE_LAYER_AMOUNT; i++)
            for (size_t j = 0; j < layers[i].size(); j++)
                for (size_t k = 0; k < layers[i][j].size(); k++)
                    layers[i][j][k].color = sf::Color::White;

    else
        differentiateLayers();
}

void LayerManager::startOver(int width, int height)
{
    activeLayer = 0;
    drawing = false;
    hightlightLayers = true;

    currentState = 0;
    prevStates.clear();

    workAreaStart = sf::Vector2i(TILEMENU_WIDTH, MENU_BAR_HEIGHT);

    layers.resize(LAYER_AMOUNT);

    for (int i = 0; i < LAYER_AMOUNT; i++)
    {
        layers[i].resize(height);

        for (int j = 0; j < height; j++)
        {
            layers[i][j].resize(width);

            for (int k = 0; k < width; k++)
            {
                layers[i][j][k].textureID = 0;
                layers[i][j][k].tileID = -1;
                layers[i][j][k].x = workAreaStart.x + (k * TILE_SIZE);
                layers[i][j][k].y = workAreaStart.y + (j * TILE_SIZE);

                if (i >= TILE_LAYER_AMOUNT)
                    layers[i][j][k].tileID = HITBOX_ID_START;
            }
        }
    }

    prevStates.push_back(layers);
}

void LayerManager::resize(int width, int height)
{
    if (width == 0)
        width++;

    if (height == 0)
        height++;

    int oldHeight = layers[0].size();
    int oldWidth = layers[0][0].size();

    //slow but easy on me brain
    auto temp = layers;

    for (int i = 0; i < LAYER_AMOUNT; i++)
    {
        layers[i].resize(height);

        for (int j = 0; j < height; j++)
            layers[i][j].resize(width);
    }

    if (width > oldWidth || height > oldHeight)
    {
        for (int i = 0; i < LAYER_AMOUNT; i++)
        {
            for (int j = 0; j < height; j++)
            {
                for (int k = 0; k < width; k++)
                {
                    layers[i][j][k].textureID = 0;
                    layers[i][j][k].tileID = -1;
                    layers[i][j][k].x = workAreaStart.x + (k * TILE_SIZE);
                    layers[i][j][k].y = workAreaStart.y + (j * TILE_SIZE);

                    if (i >= TILE_LAYER_AMOUNT)
                        layers[i][j][k].tileID = HITBOX_ID_START;
                }
            }
        }

        for (int i = 0; i < LAYER_AMOUNT; i++)
        {
            for (int j = 0; j < std::min(height, oldHeight); j++)
            {
                for (int k = 0; k < std::min(width, oldWidth); k++)
                {
                    layers[i][j][k] = temp[i][j][k];
                }
            }
        }
    }
}

void LayerManager::undo(int steps)
{
    currentState -= steps;

    if (currentState < 0)
        currentState = 0;

    if (!prevStates.empty())
        layers = prevStates[currentState];

    differentiateLayers();
}

void LayerManager::redo(int steps)
{
    if (!prevStates.empty())
    {
        currentState += steps;
        if (currentState > prevStates.size() - 1)
            currentState = (int)prevStates.size() - 1;

        else
            layers = prevStates[currentState];
    }

    differentiateLayers();
}

void LayerManager::differentiateLayers()
{
    if (hightlightLayers)
    {
        for (int i = 0; i < TILE_LAYER_AMOUNT; i++)
        {
            for (size_t j = 0; j < layers[i].size(); j++)
            {
                for (size_t k = 0; k < layers[i][j].size(); k++)
                {
                    int color = int(((i + 1.f) / (std::min(activeLayer, TILE_LAYER_AMOUNT - 1) + 1.f)) * 255);
                    int transparency = int((float(TILE_LAYER_AMOUNT - i) / (TILE_LAYER_AMOUNT - std::min(activeLayer, TILE_LAYER_AMOUNT - 1))) * 255);
                    transparency = std::min(255, transparency);
                    
                    layers[i][j][k].color = sf::Color(color, color, color, transparency);
                }
            }
        }
    }
}

void LayerManager::processImagePart(sf::Image & image, int layer, int i, int j) const
{
    if (layers[layer][i][j].tileID != -1)
        image.copy(TileMaps::get().getTexture(layers[layer][i][j].textureID).copyToImage(),
            layers[layer][i][j].x - workAreaStart.x,
            layers[layer][i][j].y - workAreaStart.y,
            TileMaps::get().getTileRect(layers[layer][i][j].textureID, layers[layer][i][j].tileID),
            true);
}

std::ostream & operator<<(std::ostream & out, const LayerManager & layerManager)
{
    out << layerManager.activeLayer << "\n";
    out << layerManager.layers[0].size() << "\n";
    out << layerManager.layers[0][0].size() << "\n";

    for (size_t i = 0; i < LAYER_AMOUNT; i++)
    {
        for (size_t j = 0; j < layerManager.layers[i].size(); j++)
        {
            for (size_t k = 0; k < layerManager.layers[i][j].size(); k++)
            {
                //total cancer probably
                out << layerManager.layers[i][j][k].textureID << " ";
                out << layerManager.layers[i][j][k].tileID << " ";
            }
        }
        out << "\n";
    }

    out << "\n";


    return out;
}

std::istream & operator>>(std::istream & in, LayerManager & layerManager)
{
    layerManager.prevStates.clear();
    layerManager.currentState = 0;

    in >> layerManager.activeLayer;
    int x = 0;
    int y = 0;
    in >> y >> x;
    for (int i = 0; i < LAYER_AMOUNT; i++)
    {
        layerManager.layers[i].resize(y);
        for (int j = 0; j < y; j++)
        {
            layerManager.layers[i][j].resize(x);
            for (int k = 0; k < x; k++)
            {
                in >> layerManager.layers[i][j][k].textureID;
                in >> layerManager.layers[i][j][k].tileID;
                layerManager.layers[i][j][k].x = layerManager.workAreaStart.x + (k * TILE_SIZE);
                layerManager.layers[i][j][k].y = layerManager.workAreaStart.y + (j * TILE_SIZE);
            }
        }
    }

    layerManager.prevStates.push_back(layerManager.layers);

    layerManager.differentiateLayers();

    return in;
}
