#include "LevelHandler.h"
#include <fstream>
#include "Misc/Definitions.h"

#define LEVEL_FOLDER "../Maps/"
#define LEVEL_TEX_FOLDER LEVEL_FOLDER "Textures/"
#define LAYER_AMOUNT 3
#define ENEMY_SPAWN_POINT 503

const std::string LEVEL_FILE_NAMES[NR_OF_LEVELS] = 
{"Level1.yay"};

const std::string LEVEL_BG_NAMES[NR_OF_LEVELS] =
{ "forestBG.png" };

LevelHandler::LevelHandler()
{
}

bool LevelHandler::loadLevel()
{
    this->importLevel(levels::forest);
    this->generateHitboxes(CollisionBox::colliderComponents::Ground);
    this->generateHitboxes(CollisionBox::colliderComponents::Platform);
    this->createSpites();
    this->generateShadowLines();

    if (!this->backgroundTexture.loadFromFile(LEVEL_TEX_FOLDER + LEVEL_BG_NAMES[levels::forest]))
        exit(-2);

    this->backgroundSprite.setTexture(backgroundTexture);

    return true;
}

void LevelHandler::updateLevel(float dt)
{
    for (auto & ter : terrain)
        CollisionHandler::queueStaticCollider(&ter);

    for (auto & line : this->shadowLines)
        ShadowHandler::queueLine(line);
}

void LevelHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(this->backgroundSprite, states);

    //TODO: only draw stuff thats on the screen
    for (size_t i = 0; i < linearSprite.size(); i++)
    {
        target.draw(linearSprite[i], states);
    }
}

void LevelHandler::drawCollision(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (size_t i = 0; i < terrain.size(); i++)
    {
        target.draw(terrain[i].getCollisionBox());
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

        this->hitboxData.resize(y);
        for (int j = 0; j < y; j++)
        {
            hitboxData[j].resize(x);
            for (int k = 0; k < x; k++)
            {
                in >> this->hitboxData[j][k].textureID;
                in >> this->hitboxData[j][k].tileID;
                this->hitboxData[j][k].x = k * TILE_SIZE;
                this->hitboxData[j][k].y = j * TILE_SIZE;
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

bool LevelHandler::generateHitboxes(CollisionBox::colliderComponents type)
{
    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());

    std::vector<std::vector<bool>> open(end.y, std::vector<bool>(end.x, true));

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == type)
            {
                bool horExisits = false;
                sf::Vector2f min = sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y);
                sf::Vector2f max = sf::Vector2f((float)hitboxData[i][j].x + TILE_SIZE, (float)hitboxData[i][j].y + TILE_SIZE);

                //Big 'ol box
                if (open[i][j])
                {
                    sf::Vector2i bounds = sf::Vector2i(1, INT_MAX);
                    bool foundYEnd = false;

                    int k = 0;
                    while (k + j < end.x && hitboxData[i][j + k].tileID == hitboxData[i][j].tileID && open[i][j + k])
                    {
                        int l = 1;
                        while (l + i < end.y && hitboxData[i + l][j + k].tileID == hitboxData[i][j + k].tileID)
                        {
                            l++;
                        }

                        bounds.y = std::min(bounds.y, l);
                        k++;
                    }
                    bounds.x = k;

                    if (bounds.x > 1 && bounds.y > 1)
                    {
                        for (int l = 0; l < bounds.y; l++)
                        {
                            for (int m = 0; m < bounds.x; m++)
                            {
                                open[i + l][j + m] = false;
                            }
                        }
                        max.y += TILE_SIZE * (bounds.y - 1);
                        max.x += TILE_SIZE * (bounds.x - 1);


                        Terrain ter(CollisionBox::AABB(min, max - min), type);
                        terrain.push_back(ter);
                    }

                }

                //Horizontal
                if (open[i][j])
                {
                    int k = 1;
                    while (k + j < end.x && hitboxData[i][j + k].tileID == hitboxData[i][j].tileID && open[i][j + k])
                    {
                        max.x += TILE_SIZE;
                        open[i][j + k] = false;
                        k++;
                        horExisits = true;
                    }

                    //if other tiles has been found
                    if (horExisits || (1 + i < end.y && hitboxData[i + 1][j].tileID != hitboxData[i][j].tileID))
                    {
                        Terrain ter(CollisionBox::AABB(min, max - min), type);
                        ter.getCollisionBox().addComponent(type);
                        terrain.push_back(ter);
                        open[i][j] = false;
                    }
                }

                //Vertical 
                if (1 + i < end.y && hitboxData[i + 1][j].tileID == hitboxData[i][j].tileID && open[i + 1][j])
                {
                    int k = 1;
                    if (horExisits || !open[i][j]) 
                    {
                        min = sf::Vector2f((float)hitboxData[i + k][j].x, (float)hitboxData[i + k][j].y);
                        max = sf::Vector2f((float)hitboxData[i + k][j].x + TILE_SIZE, (float)hitboxData[i + k][j].y + TILE_SIZE);
                        open[i + k][j] = false;
                        k++;
                    }

                    else 
                    {
                        min = sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y);
                        max = sf::Vector2f((float)hitboxData[i][j].x + TILE_SIZE, (float)hitboxData[i][j].y + TILE_SIZE);
                        open[i][j] = false;
                    }

                    while (k + i < end.y && hitboxData[i + k][j].tileID == hitboxData[i][j].tileID)
                    {
                        max.y += TILE_SIZE;
                        open[i + k][j] = false;
                        k++;
                    }


                    Terrain ter = Terrain(CollisionBox::AABB(min, max - min), type);
                    terrain.push_back(ter);
                }
            }
        }
    }


    return true;
}

std::vector<sf::Vector2f> LevelHandler::generateSpawnPoints()
{
    std::vector<sf::Vector2f> spawnPoints;

    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == ENEMY_SPAWN_POINT)
            {
                spawnPoints.push_back(sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y));

            }
        }
    }


    return spawnPoints;
}

void LevelHandler::generateShadowLines()
{
    for (auto& ter : terrain)
    {
        if (ter.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
        {
            Line top(
                ter.getPosition(),
                sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y));

            Line right(
                sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y),
                sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y));

            Line bottom(
                sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y),
                sf::Vector2f(ter.getPosition().x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y));

            Line left(sf::Vector2f(ter.getPosition().x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y),
                ter.getPosition());

            this->shadowLines.push_back(top);
            this->shadowLines.push_back(right);
            this->shadowLines.push_back(bottom);
            this->shadowLines.push_back(left);
        }
    }
}

void LevelHandler::createSpites()
{
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
                    sprite.setPosition((float)tile.x, (float)tile.y);
                    int xMap = tile.tileID % tilemaps[tile.textureID].x;
                    int yMap = tile.tileID / tilemaps[tile.textureID].x;

                    sprite.setTextureRect(sf::IntRect(xMap * TILE_SIZE, yMap * TILE_SIZE, TILE_SIZE, TILE_SIZE));
                    linearSprite.push_back(sprite);

                    this->dimensions.x = std::max(this->dimensions.x, (int)k * TILE_SIZE);
                    this->dimensions.y = std::max(this->dimensions.y, (int)j * TILE_SIZE);
                }
            }
        }
    }
}
