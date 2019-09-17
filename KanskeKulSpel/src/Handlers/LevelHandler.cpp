#include "LevelHandler.h"
#include <fstream>
#include "Misc/Definitions.h"
#include "ShadowHandler.h"

#define LEVEL_FOLDER "../Maps/"
#define LEVEL_TEX_FOLDER LEVEL_FOLDER "Textures/"
#define LAYER_AMOUNT 3


enum hitBoxTypes 
{
    standard = 501
};

const std::string LEVEL_FILE_NAMES[NR_OF_LEVELS] = 
{"Level1.yay"};

LevelHandler::LevelHandler()
{
}

bool LevelHandler::loadLevel()
{
    importLevel(levels::forest);
    generateHitboxes();

    return true;
}

void LevelHandler::updateLevel(float dt)
{
    for (auto & ter : terrain)
        CollisionHandler::queueCollider(&ter);

    //queueShadows();
}

void LevelHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //TODO: only draw stuff thats on the screen
    for (size_t i = 0; i < linearSprite.size(); i++)
    {
        target.draw(linearSprite[i]);
    }
}

void LevelHandler::drawCollision(sf::RenderWindow & window, sf::RenderStates states) const
{
    for (size_t i = 0; i < terrain.size(); i++)
    {
        window.draw(terrain[i].getCollisionBox());
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


        this->createSpites();

        return true;
    }

    return false;
}

bool LevelHandler::generateHitboxes()
{
    sf::Vector2i end = sf::Vector2i(hitboxData[0].size(), hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == hitBoxTypes::standard)
            {
                sf::Vector2f min = sf::Vector2f(hitboxData[i][j].x, hitboxData[i][j].y);
                sf::Vector2f max = sf::Vector2f(hitboxData[i][j].x, hitboxData[i][j].y + TILE_SIZE);
            
                int k = 0;
                while (k + j < end.x && hitboxData[i][j + k].tileID == hitboxData[i][j].tileID)
                {
                    k++;
                    max.x += TILE_SIZE;
                }

                j += k;

                Terrain ter(CollisionBox::AABB(min, max - min));
                terrain.push_back(ter);
            }
        }
    }


    return true;
}

void LevelHandler::queueShadows()
{
    for (auto & ter : terrain)
    {
        ShadowHandler::Line top(
            ter.getPosition(), 
            sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y));

        ShadowHandler::Line right(
            sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y),
            sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y));

        ShadowHandler::Line bottom(
            sf::Vector2f(ter.getPosition().x + ter.getCollisionBox().getAABB().size.x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y),
            sf::Vector2f(ter.getPosition().x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y));

        ShadowHandler::Line left(sf::Vector2f(ter.getPosition().x, ter.getPosition().y + ter.getCollisionBox().getAABB().size.y),
            ter.getPosition());

        ShadowHandler::queueLine(top);
        ShadowHandler::queueLine(right);
        ShadowHandler::queueLine(bottom);
        ShadowHandler::queueLine(left);
    }


    /*sf::Vector2i end = sf::Vector2i(hitboxData[0].size(), hitboxData.size());

    std::vector<std::vector<bool>> open(end.y, std::vector<bool>(end.x, true));
    std::vector<sf::Vector2i> corners;

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == hitBoxTypes::standard)
            {
                sf::Vector2f min = sf::Vector2f(hitboxData[i][j].x, hitboxData[i][j].y);
                sf::Vector2f max = sf::Vector2f(hitboxData[i][j].x, hitboxData[i][j].y + TILE_SIZE);
                corners.push_back(sf::Vector2i(i, j));

                int k = 1;
                while (k + j < end.x && hitboxData[i][j + k].tileID == hitboxData[i][j].tileID)
                {
                    k++;
                    max.x += TILE_SIZE;
                    corners.push_back(sf::Vector2i(i, j + k));

                }

                j += k;

                Terrain ter(CollisionBox::AABB(min, max - min));
                terrain.push_back(ter);
            }
        }
    }*/
}

void LevelHandler::createSpites()
{
    spriteLayers.resize(LAYER_AMOUNT);
    for (size_t i = 0; i < layers.size(); i++)
    {
        spriteLayers[i].resize(layers[i].size());
        for (size_t j = 0; j < layers[i].size(); j++)
        {
            spriteLayers[i][j].resize(layers[i][j].size());
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
                    linearSprite.push_back(sprite);
                }
            }
        }
    }
}
