#include "LevelHandler.h"
#include <fstream>
#include <sstream>
#include "Game/Misc/Definitions.h"
#include "Game/Misc/UnorderedErase.h"
#include "Game/Handlers/CharacterHandler.h"
#include "Misc/ConsoleWindow.h"

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
    m_bossInLevel = false;
    m_drawCollision = false;

    ConsoleWindow::get().addCommand("levelShowCollision", [&](Arguments args)->std::string 
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            m_drawCollision = std::stoi(args[0]);


            return "It is done";
        });
}

LevelHandler::~LevelHandler()
{
    for (Light* light : m_lights)
        delete light;
}

bool LevelHandler::loadLevel(const LevelInfo* level)
{
    m_terrain.clear();
    
    m_bossInLevel = false;

    bool success = importLevel(level);
    if (!success)
        return false;

    generateHitboxes(GROUND, ColliderKeys::ground);
    generateHitboxes(PLATFORM, ColliderKeys::platform);
    generateHitboxes(LEVEL_RETURN_POINT, ColliderKeys::levelReturn);
    generateHitboxes(LEVEL_WARP_POINT, ColliderKeys::levelWarp);


    m_exits.clear();
    m_exitBlockades.clear();

    for (CustomHitbox& box : m_customHitboxes)
    {
        if (box.flag.compare(0, 4, "boss") == 0)
        {
            m_bossInLevel = true;
            generateBossSpawner(box);
        }

        else
        {
            Terrain ter(AABB(sf::Vector2f(box.min), sf::Vector2f(box.max - box.min)), ColliderKeys::customTerrain, box.flag);
            
            if (box.flag.compare(0, 4, "exit") == 0)
            {
                m_exits.push_back(ter);
                m_exitBlockades.push_back(Terrain(AABB(sf::Vector2f(box.min), sf::Vector2f(box.max - box.min)), ColliderKeys::ground));
            }

            else
                m_terrain.push_back(ter);
        }
    }

    createSpites();


    std::vector<Terrain> staticShadowsCasters;

    for (const Terrain& ter : m_terrain)
    {
        if (ter.getColliderComp()->hasComponent(ColliderKeys::ground))
            staticShadowsCasters.push_back(ter);
    }

    m_shadowLines.clear();
    generateShadowLines(staticShadowsCasters, &m_shadowLines);

    m_exitShadowLines.clear();
    generateShadowLines(m_exits, &m_exitShadowLines);
    generateBreakableShadowLines();

    m_backgroundSprite.setTexture(m_backgroundTexture);
    return true;
}

void LevelHandler::updateLevel(float dt)
{
    for (int i = 0; i < m_breakableTerrain.size(); i++)
    {
        if (m_breakableTerrain[i].isBroken())
        {
            unordered_erase(m_breakableTerrain, m_breakableTerrain.begin() + i);
            unordered_erase(m_breakableShadowLines, m_breakableShadowLines.begin() + i--);
        }
    }
}

void LevelHandler::queueColliders()
{
    for (Terrain& ter : m_terrain)
        CollisionHandler::queueStaticCollider(&ter);

    //boss blocks all escape
    if (CharacterHandler::isBossActive())
        for (Terrain& ter : m_exitBlockades)
            CollisionHandler::queueStaticCollider(&ter);

    //Spawn exits as normal
    else
        for (Terrain& ter : m_exits)
            CollisionHandler::queueStaticCollider(&ter);

    for (BreakableTerrain& ter : m_breakableTerrain)
        CollisionHandler::queueBreakable(&ter);
}

void LevelHandler::queueLightsAndShadows()
{
    for (auto& line : m_shadowLines)
        ShadowHandler::queueLine(line);

    for (std::vector<Line>& lines : m_breakableShadowLines)
        for (Line& line : lines)
            ShadowHandler::queueLine(line);

    if (CharacterHandler::isBossActive())
        for (Line& line : m_exitShadowLines)
            ShadowHandler::queueLine(line);

    for (Light* light : m_lights)
        LightQueue::get().queue(light);
}

void LevelHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_drawCollision)
        for (size_t i = 0; i < m_terrain.size(); i++)
            target.draw(*m_terrain[i].getColliderComp(), states);

    if (m_drawCollision)
        for (size_t i = 0; i < m_exits.size(); i++)
            target.draw(*m_exits[i].getColliderComp(), states);
}

bool LevelHandler::importLevel(const LevelInfo* level)
{
    std::string trash = "";

    for (Light* light : m_lights)
        delete light;

    m_lights.clear();

    m_layers.clear();
    m_tilemaps.clear();
    m_customHitboxes.clear();
    m_layers.resize(LAYER_AMOUNT);

    std::ifstream in(LEVEL_FOLDER + level->levelFileName);
    if (in.is_open())
    {
        in >> trash;
        int x = 0;
        int y = 0;
        
        in >> y >> x;

        for (int i = 0; i < LAYER_AMOUNT; i++)
        {
            m_layers[i].resize(y);
            for (int j = 0; j < y; j++)
            {
                m_layers[i][j].resize(x);
                for (int k = 0; k < x; k++)
                {
                    in >> m_layers[i][j][k].textureID;
                    in >> m_layers[i][j][k].tileID;
                    m_layers[i][j][k].x = k * TILE_SIZE;
                    m_layers[i][j][k].y = j * TILE_SIZE;
                }
            }
        }

        m_hitboxData.resize(y);
        for (int j = 0; j < y; j++)
        {
            m_hitboxData[j].resize(x);
            for (int k = 0; k < x; k++)
            {
                in >> m_hitboxData[j][k].textureID;
                in >> m_hitboxData[j][k].tileID;
                m_hitboxData[j][k].x = k * TILE_SIZE;
                m_hitboxData[j][k].y = j * TILE_SIZE;
            }
        }

        int texCount = 0;
        in >> texCount;

        for (int i = 0; i < texCount; i++)
        {
            m_tilemaps.push_back(Tilemap());

            std::string name = "";
            in >> name;
            in >> m_tilemaps[i].x >> m_tilemaps[i].y;

            if (!m_tilemaps[i].texture.loadFromFile(LEVEL_TEX_FOLDER + name))
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

            m_lights.push_back(new Light(pos, rad, col));
        }

        int hitboxCount = 0;
        in >> hitboxCount;
        for (int i = 0; i < hitboxCount; i++)
        {
            CustomHitbox box;
            in >> box;
            box.min *= TILE_SIZE;
            box.max *= TILE_SIZE;
            m_customHitboxes.push_back(box);
        }
        in.close();
        

        return true;
    }

    return false;
}

bool LevelHandler::generateHitboxes(int id, ColliderKeys type)
{
    sf::Vector2i end = sf::Vector2i((int)m_hitboxData[0].size(), (int)m_hitboxData.size());

    std::vector<std::vector<bool>> open(end.y, std::vector<bool>(end.x, true));

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (m_hitboxData[i][j].tileID == id)
            {
                bool horExisits = false;
                sf::Vector2f min = sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y);
                sf::Vector2f max = sf::Vector2f((float)m_hitboxData[i][j].x + TILE_SIZE, (float)m_hitboxData[i][j].y + TILE_SIZE);

                //Big 'ol box
                if (open[i][j])
                {
                    sf::Vector2i bounds = sf::Vector2i(1, INT_MAX);
                    bool foundYEnd = false;

                    int k = 0;
                    while (k + j < end.x && m_hitboxData[i][j + k].tileID == m_hitboxData[i][j].tileID && open[i][j + k])
                    {
                        int l = 1;
                        while (l + i < end.y && m_hitboxData[i + l][j + k].tileID == m_hitboxData[i][j + k].tileID)
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
                        m_terrain.push_back(ter);
                    }

                }

                //Horizontal
                if (open[i][j])
                {
                    int k = 1;
                    while (k + j < end.x && m_hitboxData[i][j + k].tileID == m_hitboxData[i][j].tileID && open[i][j + k])
                    {
                        max.x += TILE_SIZE;
                        open[i][j + k] = false;
                        k++;
                        horExisits = true;
                    }

                    //if other tiles has been found
                    if (horExisits || (1 + i < end.y && m_hitboxData[i + 1][j].tileID != m_hitboxData[i][j].tileID))
                    {
                        Terrain ter(AABB(min, max - min), type);
                        m_terrain.push_back(ter);
                        open[i][j] = false;
                    }
                }

                //Vertical 
                if (1 + i < end.y && m_hitboxData[i + 1][j].tileID == m_hitboxData[i][j].tileID && open[i + 1][j])
                {
                    int k = 1;
                    if (horExisits || !open[i][j]) 
                    {
                        min = sf::Vector2f((float)m_hitboxData[i + k][j].x, (float)m_hitboxData[i + k][j].y);
                        max = sf::Vector2f((float)m_hitboxData[i + k][j].x + TILE_SIZE, (float)m_hitboxData[i + k][j].y + TILE_SIZE);
                        open[i + k][j] = false;
                        k++;
                    }

                    else 
                    {
                        min = sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y);
                        max = sf::Vector2f((float)m_hitboxData[i][j].x + TILE_SIZE, (float)m_hitboxData[i][j].y + TILE_SIZE);
                        open[i][j] = false;
                    }

                    while (k + i < end.y && m_hitboxData[i + k][j].tileID == m_hitboxData[i][j].tileID)
                    {
                        max.y += TILE_SIZE;
                        open[i + k][j] = false;
                        k++;
                    }


                    Terrain ter = Terrain(AABB(min, max - min), type);
                    m_terrain.push_back(ter);
                }
            }
        }
    }


    return true;
}

std::vector<sf::Vector2f> LevelHandler::generateEnemySpawnPoints()
{
    std::vector<sf::Vector2f> spawnPoints;

    sf::Vector2i end = sf::Vector2i((int)m_hitboxData[0].size(), (int)m_hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (m_hitboxData[i][j].tileID == ENEMY_SPAWN_POINT)
            {
                spawnPoints.push_back(sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y));
            }
        }
    }


    return spawnPoints;
}

std::vector<sf::Vector2f> LevelHandler::generateGatherPoints()
{
    std::vector<sf::Vector2f> gatherPoints;

    sf::Vector2i end = sf::Vector2i((int)m_hitboxData[0].size(), (int)m_hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (m_hitboxData[i][j].tileID == GATHER_POINT)
            {
                gatherPoints.push_back(sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y));
            }
        }
    }


    return gatherPoints;
}

std::vector<sf::Vector2f> LevelHandler::generateRareGatherPoints()
{
    std::vector<sf::Vector2f> gatherPoints;

    sf::Vector2i end = sf::Vector2i((int)m_hitboxData[0].size(), (int)m_hitboxData.size());

    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (m_hitboxData[i][j].tileID == RARE_GATHER_POINT)
            {
                gatherPoints.push_back(sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y));
            }
        }
    }


    return gatherPoints;
}

std::vector<CustomHitbox> LevelHandler::getShrines()
{
    std::vector<CustomHitbox> shrines;

    for (CustomHitbox& box : m_customHitboxes)
        if (box.flag.compare(0, 6, "shrine") == 0)
            shrines.push_back(box);
    return shrines;
}

sf::Vector2f LevelHandler::findPlayerSpawnPoints(int exitTaken)
{
    sf::Vector2i end = sf::Vector2i((int)m_hitboxData[0].size(), (int)m_hitboxData.size());
    sf::Vector2f point;

    std::vector<sf::Vector2f> points;
    for (int i = 0; i < end.y; i++)
    {
        for (int j = 0; j < end.x; j++)
        {
            if (m_hitboxData[i][j].tileID == PLAYER_SPAWN_POINT)
            {
                points.push_back(sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y));
            }
        }
    }

    point = points[0];

    if (exitTaken == -1)
    {
        for (int i = 0; i < end.y; i++)
        {
            for (int j = 0; j < end.x; j++)
            {
                if (m_hitboxData[i][j].tileID == LEVEL_WARP_POINT)
                    point = sf::Vector2f((float)m_hitboxData[i][j].x, (float)m_hitboxData[i][j].y);
            }
        }
    }

    else
    {
        sf::Vector2f entrance;

        for (const CustomHitbox& box : m_customHitboxes)
        {
            if (box.flag[4] - '0' == exitTaken)
                entrance = sf::Vector2f(box.min);
        }

        for (sf::Vector2f p : points)
            if (lengthSquared(point - entrance) > lengthSquared(p - entrance))
                point = p;
    }

    return point;
}

void LevelHandler::generateShadowLines(const std::vector<Terrain>& terrain, std::vector<Line>* vec)
{
    for (const Terrain& ter : terrain)
    {
        const ColliderComp* collider = ter.getColliderComp();
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

        vec->push_back(top);
        vec->push_back(right);
        vec->push_back(bottom);
        vec->push_back(left);
    }
}

void LevelHandler::generateBreakableShadowLines()
{
    m_breakableShadowLines.clear();

    for (auto& ter : m_breakableTerrain)
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


            m_breakableShadowLines.push_back(lines);
        }
    }
}

void LevelHandler::createSpites()
{
    m_linearSprite.clear();
    m_breakableTerrain.clear();
    for (size_t i = 0; i < m_layers.size(); i++)
    {
        for (size_t j = 0; j < m_layers[i].size(); j++)
        {
            for (size_t k = 0; k < m_layers[i][j].size(); k++)
            {
                Tile tile = m_layers[i][j][k];
                if (tile.tileID != -1)
                {
                    int xMap = tile.tileID % m_tilemaps[tile.textureID].x;
                    int yMap = tile.tileID / m_tilemaps[tile.textureID].x;
                    sf::Vector2f pos((float)tile.x, (float)tile.y);
                    sf::IntRect rect(xMap * TILE_SIZE, yMap * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                    
                    m_dimensions.x = std::max(m_dimensions.x, (int)k * TILE_SIZE);
                    m_dimensions.y = std::max(m_dimensions.y, (int)j * TILE_SIZE);

                    if (m_hitboxData[j][k].tileID != BREAKABLE_BLOCK || i == 0)
                    {
                        sf::Sprite sprite;
                        sprite.setTexture(m_tilemaps[tile.textureID].texture);
                        sprite.setPosition(pos);


                        sprite.setTextureRect(rect);
                        m_linearSprite.push_back(sprite);


                    }

                    else
                    {
                        if (i == 1) //layer 2
                        {
                            BreakableTerrain ter(pos, &m_tilemaps[tile.textureID].texture, rect);
                            if (m_layers[2][j][k].tileID != -1)
                            {
                                int xOverMap = tile.tileID % m_tilemaps[m_layers[2][j][k].textureID].x;
                                int yOverMap = tile.tileID / m_tilemaps[m_layers[2][j][k].textureID].x;
                                sf::IntRect rectOverlay(xOverMap * TILE_SIZE, yOverMap * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                                ter.addOverlay(&m_tilemaps[m_layers[2][j][k].textureID].texture, rectOverlay);
                            }

                            m_breakableTerrain.push_back(ter);
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
    m_bossSpawner.playerItemCriteria.clear();
    m_bossSpawner.pos = sf::Vector2f(box.min);
    m_bossSpawner.playerPosCriteria = sf::FloatRect(sf::Vector2f(box.min), sf::Vector2f(box.max));

    //Boss flag
    sstream >> str;

    sstream >> str;
    m_bossSpawner.bossID = std::stoi(str);

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

            m_bossSpawner.playerItemCriteria.push_back(item);
        }
    }
}

void LevelHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(backgroundSprite, states);

    //TODO: only draw stuff thats on the screen
    for (size_t i = 0; i < m_linearSprite.size(); i++)
    {
        target.draw(m_linearSprite[i], states);
    }

    for (const BreakableTerrain& ter : m_breakableTerrain)
        target.draw(ter, states);
}

