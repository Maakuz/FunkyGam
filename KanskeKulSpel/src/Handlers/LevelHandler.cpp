#include "LevelHandler.h"
#include <fstream>

#define LEVEL_FOLDER "../Maps/"
#define LEVEL_TEX_FOLDER LEVEL_FOLDER "Textures/"
#define LAYER_AMOUNT 3
#define TILE_SIZE 32

const std::string LEVEL_FILE_NAMES[NR_OF_LEVELS] = 
{"TestMap.yay"};

LevelHandler::LevelHandler()
{
}

bool LevelHandler::loadLevel()
{
    importLevel(levels::forest);


    return true;
}

void LevelHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    //TODO: only draw stuff thats on the screen
    std::vector<sf::Sprite> renderQueue;

    for (size_t i = 0; i < layers.size(); i++)
    {
        for (size_t j = 0; j < layers[i].size(); j++)
        {
            for (size_t k = 0; k < layers[i][j].size(); k++)
            {
                Tile tile = layers[i][j][k];
                if (tile.tileID != -1)
                {
                    sf::Sprite sprite;
                    sprite.setTexture(tilemaps[tile.textureID].texture);
                    sprite.setPosition(tile.x, tile.y);
                    int xMap = tile.tileID % tilemaps[tile.textureID].x;
                    int yMap = tile.tileID / tilemaps[tile.textureID].x;

                    sprite.setTextureRect(sf::IntRect(xMap * TILE_SIZE, yMap * TILE_SIZE, TILE_SIZE, TILE_SIZE));
                    renderQueue.push_back(sprite);
                }
            }
        }
    }

    for (size_t i = 0; i < renderQueue.size(); i++)
    {
        target.draw(renderQueue[i]);
    }
}

bool LevelHandler::importLevel(levels level)
{
    std::string trash = "";

    layers.clear();
    tilemaps.clear();
    layers.resize(LAYER_AMOUNT);

    std::ifstream in(LEVEL_FOLDER + LEVEL_FILE_NAMES[level]);
    if (in.is_open())
    {
        in >> trash;
        int x = 0;
        int y = 0;
        in >> y >> x;
        for (int i = 0; i < LAYER_AMOUNT; i++)
        {
            this->layers[i].resize(y);
            for (int j = 0; j < y; j++)
            {
                layers[i][j].resize(x);
                for (int k = 0; k < x; k++)
                {
                    in >> this->layers[i][j][k].textureID;
                    in >> this->layers[i][j][k].tileID;
                    this->layers[i][j][k].x = k * TILE_SIZE;
                    this->layers[i][j][k].y = j * TILE_SIZE;
                }
            }
        }

        int texCount = 0;
        in >> texCount;

        for (int i = 0; i < texCount; i++)
        {
            tilemaps.push_back(Tilemap());

            std::string name = "";
            in >> name;
            in >> tilemaps[i].x >> tilemaps[i].y;

            if (!tilemaps[i].texture.loadFromFile(LEVEL_TEX_FOLDER + name))
                exit(-4);
        }

        in.close();


        

        return true;
    }

    return false;
}
