#include "LevelHandler.h"
#include <fstream>
#include <sstream>
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/UnorderedErase.h"

#define LEVEL_FOLDER "../Resources/Maps/"
#define LEVEL_TEX_FOLDER LEVEL_FOLDER "Textures/"
#define LAYER_AMOUNT 3
#define GROUND 501
#define PLATFORM 502
#define ENEMY_SPAWN_POINT 503
#define GATHER_POINT 504
#define RARE_GATHER_POINT 505
#define PLAYER_SPAWN_POINT 506
#define LEVEL_WARP_POINT 507
#define LEVEL_RETURN_POINT 508
#define BREAKABLE_BLOCK 509
#define CUSTOM_TERRAIN 999

LevelHandler::LevelHandler()
{
    this->bossInLevel = false;
    this->drawCollision = false;

    ConsoleWindow::get().addCommand("levelShowCollision", [&](Arguments args)->std::string 
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawCollision = std::stoi(args[0]);


            return "It is done";
        });
}

LevelHandler::~LevelHandler()
{
    for (Light* light : lights)
        delete light;
}

bool LevelHandler::loadLevel(const LevelInfo* level)
{
    terrain.clear();
    bossInLevel = false;

    bool success = this->importLevel(level);
    if (!success)
        return false;

    this->generateHitboxes(GROUND, ColliderKeys::ground);
    this->generateHitboxes(PLATFORM, ColliderKeys::platform);
    this->generateHitboxes(LEVEL_RETURN_POINT, ColliderKeys::levelReturn);
    this->generateHitboxes(LEVEL_WARP_POINT, ColliderKeys::levelWarp);

    for (CustomHitbox& box : customHitboxes)
    {
        if (box.flag.compare(0, 4, "boss") == 0)
        {
            bossInLevel = true;
            generateBossSpawner(box);
        }

        else
        {
            Terrain ter(AABB(sf::Vector2f(box.min), sf::Vector2f(box.max - box.min)), ColliderKeys::customTerrain, box.flag);
            terrain.push_back(ter);
        }
    }
    this->createSpites();
    this->generateShadowLines();

    //if (!this->backgroundTexture.loadFromFile(LEVEL_TEX_FOLDER + LEVEL_BG_NAMES[level]))
    //{
    //    exit(-55);
    //    system("Pause");
    //}

    this->backgroundSprite.setTexture(backgroundTexture);
    return true;
}

void LevelHandler::updateLevel(float dt)
{
    for (int i = 0; i < breakableTerrain.size(); i++)
    {
        if (breakableTerrain[i].isBroken())
        {
            unordered_erase(breakableTerrain, breakableTerrain.begin() + i);
            unordered_erase(breakableShadowLines, breakableShadowLines.begin() + i--);
        }
    }
}

void LevelHandler::queueColliders()
{
    for (auto& ter : terrain)
        CollisionHandler::queueStaticCollider(&ter);

    for (auto& ter : breakableTerrain)
        CollisionHandler::queueBreakable(&ter);
}

void LevelHandler::queueLightsAndShadows()
{
    for (auto& line : this->shadowLines)
        ShadowHandler::queueLine(line);

    for (std::vector<Line>& lines : this->breakableShadowLines)
        for (Line& line : lines)
            ShadowHandler::queueLine(line);

    for (Light* light : lights)
        LightQueue::get().queue(light);
}

void LevelHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (drawCollision)
        for (size_t i = 0; i < terrain.size(); i++)
            target.draw(*terrain[i].getColliderComp(), states);
}

bool LevelHandler::importLevel(const LevelInfo* level)
{
    std::string trash = "";

    for (Light* light : lights)
        delete light;

    lights.clear();

    layers.clear();
    tilemaps.clear();
    customHitboxes.clear();
    layers.resize(LAYER_AMOUNT);

    std::ifstream in(LEVEL_FOLDER + level->levelFileName);
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
            {
                system("Pause");
                exit(-3);
            }
        }


        int lightCount = 0;
        in >> lightCount;

        for (int i = 0; i < lightCount; i++)
        {
            sf::Vector2f pos;
            sf::Vector2f offset(336, 20);
            float rad;
            sf::Vector3f col;

            in >> pos.x >> pos.y;
            in >> rad;
            in >> col.x >> col.y >> col.z;

            lights.push_back(new Light(pos, rad, col));
        }

        int hitboxCount = 0;
        in >> hitboxCount;
        for (int i = 0; i < hitboxCount; i++)
        {
            CustomHitbox box;
            in >> box;
            box.min *= TILE_SIZE;
            box.max *= TILE_SIZE;
            customHitboxes.push_back(box);
        }
        in.close();
        

        return true;
    }

    return false;
}

bool LevelHandler::generateHitboxes(int id, ColliderKeys type)
{
    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());

    std::vector<std::vector<bool>> open(end.y, std::vector<bool>(end.x, true));

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == id)
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


                        Terrain ter(AABB(min, max - min), type);
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
                        Terrain ter(AABB(min, max - min), type);
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


                    Terrain ter = Terrain(AABB(min, max - min), type);
                    terrain.push_back(ter);
                }
            }
        }
    }


    return true;
}

std::vector<sf::Vector2f> LevelHandler::generateEnemySpawnPoints()
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

std::vector<sf::Vector2f> LevelHandler::generateGatherPoints()
{
    std::vector<sf::Vector2f> gatherPoints;

    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == GATHER_POINT)
            {
                gatherPoints.push_back(sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y));
            }
        }
    }


    return gatherPoints;
}

std::vector<sf::Vector2f> LevelHandler::generateRareGatherPoints()
{
    std::vector<sf::Vector2f> gatherPoints;

    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (hitboxData[i][j].tileID == RARE_GATHER_POINT)
            {
                gatherPoints.push_back(sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y));
            }
        }
    }


    return gatherPoints;
}

std::vector<CustomHitbox> LevelHandler::getShrines()
{
    std::vector<CustomHitbox> shrines;

    for (CustomHitbox& box : customHitboxes)
        if (box.flag.compare(0, 6, "shrine") == 0)
            shrines.push_back(box);
    return shrines;
}

sf::Vector2f LevelHandler::findPlayerSpawnPoints(int exitTaken)
{
    sf::Vector2i end = sf::Vector2i((int)hitboxData[0].size(), (int)hitboxData.size());
    sf::Vector2f point;


    if (exitTaken == -1)
    {
        for (int i = 0; i < end.y; i++)
        {
            for (int j = 0; j < end.x; j++)
            {
                if (hitboxData[i][j].tileID == LEVEL_WARP_POINT)
                {
                    point = sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y);
                }
            }
        }
    }

    else
    {
        sf::Vector2f entrance;

        for (const CustomHitbox& box : customHitboxes)
        {
            if (box.flag[4] - '0' == exitTaken)
                entrance = sf::Vector2f(box.min);
        }

        std::vector<sf::Vector2f> points;
        for (int i = 0; i < end.y; i++)
        {
            for (int j = 0; j < end.x; j++)
            {
                if (hitboxData[i][j].tileID == PLAYER_SPAWN_POINT)
                {
                    points.push_back(sf::Vector2f((float)hitboxData[i][j].x, (float)hitboxData[i][j].y));
                }
            }
        }

        point = points[0];
        for (sf::Vector2f p : points)
            if (lengthSquared(point - entrance) > lengthSquared(p - entrance))
                point = p;
    }
    return point;
}

void LevelHandler::generateShadowLines()
{
    shadowLines.clear();
    breakableShadowLines.clear();
    for (auto& ter : terrain)
    {
        const ColliderComp* collider = ter.getColliderComp();
        if (collider->hasComponent(ColliderKeys::ground))
        {
            Line top(
                collider->getAABB().pos,
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y));

            Line right(
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y),
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y + collider->getAABB().size.y));

            Line bottom(
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y + collider->getAABB().size.y),
                sf::Vector2f(collider->getAABB().pos.x, collider->getAABB().pos.y + collider->getAABB().size.y));

            Line left(sf::Vector2f(collider->getAABB().pos.x, collider->getAABB().pos.y + collider->getAABB().size.y),
                collider->getAABB().pos);

            this->shadowLines.push_back(top);
            this->shadowLines.push_back(right);
            this->shadowLines.push_back(bottom);
            this->shadowLines.push_back(left);
        }
    }

    //Might be a criminal now
    for (auto& ter : breakableTerrain)
    {
        const ColliderComp* collider = ter.getColliderComp();

        if (collider->hasComponent(ColliderKeys::ground))
        {            
             Line top(
                collider->getAABB().pos,
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y));

            Line right(
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y),
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y + collider->getAABB().size.y));

            Line bottom(
                sf::Vector2f(collider->getAABB().pos.x + collider->getAABB().size.x, collider->getAABB().pos.y + collider->getAABB().size.y),
                sf::Vector2f(collider->getAABB().pos.x, collider->getAABB().pos.y + collider->getAABB().size.y));

            Line left(sf::Vector2f(collider->getAABB().pos.x, collider->getAABB().pos.y + collider->getAABB().size.y),
                collider->getAABB().pos);


            std::vector<Line> lines;
            lines.push_back(top);
            lines.push_back(right);
            lines.push_back(bottom);
            lines.push_back(left);


            this->breakableShadowLines.push_back(lines);
        }
    }
}

void LevelHandler::createSpites()
{
    linearSprite.clear();
    breakableTerrain.clear();
    for (size_t i = 0; i < layers.size(); i++)
    {
        for (size_t j = 0; j < layers[i].size(); j++)
        {
            for (size_t k = 0; k < layers[i][j].size(); k++)
            {
                Tile tile = layers[i][j][k];
                if (tile.tileID != -1)
                {
                    int xMap = tile.tileID % tilemaps[tile.textureID].x;
                    int yMap = tile.tileID / tilemaps[tile.textureID].x;
                    sf::Vector2f pos((float)tile.x, (float)tile.y);
                    sf::IntRect rect(xMap * TILE_SIZE, yMap * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                    
                    this->dimensions.x = std::max(this->dimensions.x, (int)k * TILE_SIZE);
                    this->dimensions.y = std::max(this->dimensions.y, (int)j * TILE_SIZE);

                    if (hitboxData[j][k].tileID != BREAKABLE_BLOCK || i == 0)
                    {
                        sf::Sprite sprite;
                        sprite.setTexture(tilemaps[tile.textureID].texture);
                        sprite.setPosition(pos);


                        sprite.setTextureRect(rect);
                        linearSprite.push_back(sprite);


                    }

                    else
                    {
                        if (i == 1) //layer 2
                        {
                            BreakableTerrain ter(pos, &tilemaps[tile.textureID].texture, rect);
                            if (layers[2][j][k].tileID != -1)
                            {
                                int xOverMap = tile.tileID % tilemaps[layers[2][j][k].textureID].x;
                                int yOverMap = tile.tileID / tilemaps[layers[2][j][k].textureID].x;
                                sf::IntRect rectOverlay(xOverMap * TILE_SIZE, yOverMap * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                                ter.addOverlay(&tilemaps[layers[2][j][k].textureID].texture, rectOverlay);
                            }

                            breakableTerrain.push_back(ter);
                        }
                    }
                }
            }
        }
    }
}

void LevelHandler::generateBossSpawner(const CustomHitbox& box)
{
    std::stringstream sstream(box.flag);
    std::string str;
    this->bossSpawner.playerItemCriteria.clear();
    this->bossSpawner.pos = sf::Vector2f(box.min);
    this->bossSpawner.playerPosCriteria = sf::FloatRect(sf::Vector2f(box.min), sf::Vector2f(box.max));

    //Boss flag
    sstream >> str;

    sstream >> str;
    this->bossSpawner.bossID = std::stoi(str);

    while (!sstream.eof())
    {
        sstream >> str;

        if (str == "has")
        {
            sstream >> str;
            std::string item = str;
            sstream >> str;
            while (str != ";")
            {
                item += " ";
                item += str;
                sstream >> str;
            }

            this->bossSpawner.playerItemCriteria.push_back(item);
        }
    }
}

void LevelHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(this->backgroundSprite, states);

    //TODO: only draw stuff thats on the screen
    for (size_t i = 0; i < linearSprite.size(); i++)
    {
        target.draw(linearSprite[i], states);
    }

    for (const BreakableTerrain& ter : breakableTerrain)
        target.draw(ter, states);
}

