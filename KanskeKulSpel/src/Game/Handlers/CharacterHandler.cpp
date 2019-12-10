#include "CharacterHandler.h"
#include "Game/Collision/CollisionHandler.h"
#include "TextureHandler.h"
#include "Game/Entities/Enemies/Grunt.h"
#include "Game/Entities/Enemies/Bosses/FishMonger.h"
#include "Game/Entities/Enemies/Bird.h"
#include "Game/Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/UnorderedErase.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Misc/Profiler.h"
#include "Game/Misc/Definitions.h"
#include "Game/Item/ItemHandler.h"

const std::string CharacterHandler::ENEMIES[ENEMY_TEMPLATE_COUNT] = { "grunt.mop", "bird.mop"};
const std::string CharacterHandler::BOSSES[BOSS_TEMPLATE_COUNT] = {"fishmonger.mop" };
bool CharacterHandler::s_bossActive = false;

CharacterHandler::CharacterHandler(UIHandler* uiHandler)
{
    m_player = nullptr;
    m_occluders = nullptr;
    m_drawHitboxes = false;
    m_drawSightlines = false;
    m_boss = nullptr;
    m_bossSpawner = nullptr;

    m_ui = uiHandler;

    loadPlayer();
    loadEnemies();

    ConsoleWindow::get().addCommand("charReloadEnemies", [&](Arguments args)->std::string {
        loadEnemies();

        return "Enemies has been reloaded.";
        });

    ConsoleWindow::get().addCommand("charReloadPlayer", [&](Arguments args)->std::string {
        loadPlayer();
        

        return "Player has been reloaded.";
        });

    ConsoleWindow::get().addCommand("charHitboxes", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            m_drawHitboxes = std::stoi(args[0]);


            return "Hitboxes on mby";
        });

    ConsoleWindow::get().addCommand("charSightlines", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            m_drawSightlines = std::stoi(args[0]);


            return "Hitboxes on mby";
        });
}

CharacterHandler::~CharacterHandler()
{
    if (m_player)
        delete m_player;

    if (m_boss)
        delete m_boss;

    for (Enemy* enemy : m_enemies)
        delete enemy;

    for (Enemy* enemy : m_enemyTemplates)
        delete enemy;

    for (Boss* boss : m_bossTemplates)
        delete boss;
}

void CharacterHandler::initializeLevel(const std::vector<Line>* occluders, sf::Vector2f playerSpawnPoint)
{
    CharacterHandler::s_bossActive = false;
    m_bossSpawner = nullptr;
    if (m_boss)
    {
        delete m_boss;
        m_boss = nullptr;
    }

    m_occluders = occluders;
    m_player->reset(playerSpawnPoint - m_player->getComponent<ColliderComp>()->getSize());
}

void CharacterHandler::loadPlayer()
{
    if (m_player)
        delete m_player;

    std::ifstream file(DATA_PATH "Player.mop");
    std::string trash;

    if (!file.is_open())
    {
        exit(-55);
        system("Pause");
    }
    int textureID;
    sf::Vector2u frameCount;
    int animCount;
    std::vector<Animation> animations;

    file >> trash;
    file >> trash >> textureID;
    file >> trash >> frameCount.x >> frameCount.y;
    file >> trash >> animCount;

    for (int i = 0; i < animCount; i++)
    {
        sf::Vector2u start;
        sf::Vector2u stop;
        sf::Vector2u idle;
        float speed;
        bool startIdle;

        file >> trash;
        file >> trash >> start.x >> start.y;
        file >> trash >> stop.x >> stop.y;
        file >> trash >> idle.x >> idle.y;
        file >> trash >> speed;
        file >> trash >> startIdle;

        animations.push_back(Animation(start, stop, speed, idle, startIdle));
    }

    AnimationData data(TextureHandler::get().getTexture(textureID), frameCount, animations);


    m_player = new Player(data, m_ui, sf::Vector2f(0, 0), sf::Vector2f(32, 92));

    file >> *m_player;


    file.close();
}

void CharacterHandler::loadEnemies()
{
    for (Enemy* enemy : m_enemyTemplates)
        delete enemy;

    m_enemyTemplates.clear();

    for (int i = 0; i < ENEMY_TEMPLATE_COUNT; i++)
    {
        std::ifstream file(DATA_PATH + ENEMIES[i]);
        std::string trash;

        if (!file.is_open())
            exit(-55);

        int textureID;
        sf::Vector2u frameCount;
        int animCount;
        std::vector<Animation> animations;

        file >> trash;
        file >> trash >> textureID;
        file >> trash >> frameCount.x >> frameCount.y;
        file >> trash >> animCount;

        for (int i = 0; i < animCount; i++)
        {
            sf::Vector2u start;
            sf::Vector2u stop;
            sf::Vector2u idle;
            float speed;
            bool startIdle;

            file >> trash;
            file >> trash >> start.x >> start.y;
            file >> trash >> stop.x >> stop.y;
            file >> trash >> idle.x >> idle.y;
            file >> trash >> speed;
            file >> trash >> startIdle;

            animations.push_back(Animation(start, stop, speed, idle, startIdle));
        }

        AnimationData data(TextureHandler::get().getTexture(textureID), frameCount, animations);

        sf::Vector2f size;
        sf::Vector2f offset;

        file >> trash;
        file >> trash >> size.x >> size.y;
        file >> trash >> offset.x >> offset.y;

        Enemy* enemy;

        switch (i)
        {
        case EnemyType::grunt:
            enemy = createEnemy<Grunt>(data, size, offset);
            break;
        case EnemyType::bird:
            enemy = createEnemy<Bird>(data, size, offset);
            break;
        default:
            enemy = createEnemy<Grunt>(data, size, offset);
            break;
        }


        enemy->setEyeLevel(sf::Vector2f(size.x * 0.5f, size.y * 0.2f));

        file >> *enemy;

        file.close();
        m_enemyTemplates.push_back(enemy);
    }

    //Bosses get the same treatment
    for (Boss* boss : m_bossTemplates)
        delete boss;

    m_bossTemplates.clear();

    for (int i = 0; i < BOSS_TEMPLATE_COUNT; i++)
    {
        std::ifstream file(DATA_PATH + BOSSES[i]);
        std::string trash;

        if (!file.is_open())
            exit(-55);

        int textureID;
        sf::Vector2u frameCount;
        int animCount;
        std::vector<Animation> animations;

        file >> trash;
        file >> trash >> textureID;
        file >> trash >> frameCount.x >> frameCount.y;
        file >> trash >> animCount;

        for (int i = 0; i < animCount; i++)
        {
            sf::Vector2u start;
            sf::Vector2u stop;
            sf::Vector2u idle;
            float speed;
            bool startIdle;

            file >> trash;
            file >> trash >> start.x >> start.y;
            file >> trash >> stop.x >> stop.y;
            file >> trash >> idle.x >> idle.y;
            file >> trash >> speed;
            file >> trash >> startIdle;

            animations.push_back(Animation(start, stop, speed, idle, startIdle));
        }

        AnimationData data(TextureHandler::get().getTexture(textureID), frameCount, animations);

        sf::Vector2f size;
        sf::Vector2f offset;

        file >> trash;
        file >> trash >> size.x >> size.y;
        file >> trash >> offset.x >> offset.y;

        Boss* boss;

        switch (i)
        {
        case BossType::fishmonger:
            boss = createEnemy<FishMonger>(data, size, offset);
            break;

        default:
            boss = createEnemy<FishMonger>(data, size, offset);
            break;
        }

        file >> *boss;

        file.close();
        m_bossTemplates.push_back(boss);
    }
}

void CharacterHandler::spawnEnemies(const LevelInfo* info)
{
    for (Enemy* enemy : m_enemies)
        delete enemy;

    m_enemies.clear();

    for (const sf::Vector2f& point : m_spawnPoints)
    {
        int i = rand() % info->enemies.size();
        
        Enemy* enemy = spawnEnemy(info->enemies[i]);
        enemy->spawn(point - sf::Vector2f(0, enemy->getColliderComp()->getSize().y));
        m_enemies.push_back(enemy);
    }
}

void CharacterHandler::update(float dt, sf::Vector2f mousePos)
{
    if (!m_boss && m_bossSpawner)
    {
        bool spawn = true;
        if (!m_bossSpawner->playerPosCriteria.contains(m_player->getComponent<MovementComp>()->transform.pos))
            spawn = false;

        if (spawn)
        {
            for (const std::string& str : m_bossSpawner->playerItemCriteria)
            {
                if (!ItemHandler::isOneTimeItemPickedUp(str))
                    spawn = false;
            }
        }

        if (spawn)
        {
            spawnBoss((BossType)m_bossSpawner->bossID, m_bossSpawner->pos);
            CharacterHandler::s_bossActive = true;
        }
    }

    if (m_boss)
    {
        m_boss->update(dt, m_player->getComponent<ColliderComp>()->getCenterPos());
        m_ui->displayEnemyDamage(m_boss->getHealthComp()->getHealthPercentage());

        if (!m_boss->isAlive())
        {
            s_bossActive = false;
            delete m_boss;
            m_boss = nullptr;
        }
    }

    m_player->update(dt, mousePos);
    
    for (int i = 0; i < m_enemies.size(); i++)
    {
        Enemy* enemy = m_enemies[i];
        if (enemy->isAlive())
        {
            if (enemy->getState() == AIComp::State::idle || 
                enemy->getState() == AIComp::State::chasing ||
                enemy->getState() == AIComp::State::searching ||
                enemy->getState() == AIComp::State::returning)
                updateEnemyLineOfSight(enemy);

            if (enemy->isHealthChanged())
                m_ui->displayEnemyDamage(enemy->getHealthComp()->getHealthPercentage());

            enemy->update(dt);
        }

        else
        {
            delete enemy;
            unordered_erase(m_enemies, m_enemies.begin() + i--);
            m_ui->hideEnemyDamage();
        }
    }
}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(m_player);

    for (Enemy* enemy : m_enemies)
        CollisionHandler::queueCollider(enemy);

    if (m_boss)
    {
        CollisionHandler::queueCollider(m_boss);
        m_boss->queueHitboxes();
    }
}

void CharacterHandler::setBossSpawner(BossSpawner* bossSpawner) 
{ 
    m_bossSpawner = bossSpawner;
}

void CharacterHandler::spawnBoss(BossType bossType, sf::Vector2f pos)
{
    if (m_boss)
        delete m_boss;

    switch (bossType)
    {
    case BossType::fishmonger:
        m_boss = new FishMonger(*dynamic_cast<FishMonger*>(m_bossTemplates[BossType::fishmonger]));
        break;
    default:
        m_boss = new FishMonger(*dynamic_cast<FishMonger*>(m_bossTemplates[BossType::fishmonger]));
        break;
    }

    m_boss->spawn(pos - sf::Vector2f(0, m_boss->getColliderComp()->getSize().y));
}

Enemy* CharacterHandler::spawnEnemy(int enemyType)
{
    Enemy* enemy;
    switch (enemyType)
    {
    case EnemyType::grunt:
        enemy = new Grunt(*dynamic_cast<Grunt*>(m_enemyTemplates[EnemyType::grunt]));
        break;
    case EnemyType::bird:
        enemy = new Bird(*dynamic_cast<Bird*>(m_enemyTemplates[EnemyType::bird]));
        break;
    default:
        enemy = new Grunt(*dynamic_cast<Grunt*>(m_enemyTemplates[EnemyType::grunt]));
        break;
    }

    return enemy;
}

//Shadows are not considered at all and that might be a good thing
void CharacterHandler::calculatePlayerIllumination()
{
    PROFILER_START("Calculate");
    sf::Vector3f illumination(0, 0, 0);
    for (int i = 0; i < LightQueue::get().getQueue().size(); i++)
    {
        Light* light = LightQueue::get().getQueue()[i];
        bool playerOccluded = false;
        sf::Vector2f dir = m_player->getComponent<ColliderComp>()->getCenterPos() - light->pos;
        float distance = length(dir);
        normalize(dir);
        if (light->radius < distance)
            playerOccluded = true;

        for (size_t j = 0; j < m_occluders->size() && !playerOccluded; j++)
        {
            float t = findIntersectionPoint(light->pos, dir, m_occluders->at(j).p1, m_occluders->at(j).p2);
            if (abs(t + 1) > EPSYLONE && t < distance)
            {
                playerOccluded = true;
            }
        }

        if (!playerOccluded)
        {
            float percentage = 1 - (distance / light->radius);
            illumination.x += (percentage * light->color.x);
            illumination.y += (percentage * light->color.y);
            illumination.z += (percentage * light->color.z);
        }
    }
    illumination.x = std::min(illumination.x, 1.f);
    illumination.y = std::min(illumination.y, 1.f);
    illumination.z = std::min(illumination.z, 1.f);

    float finalIllumination = illumination.x + illumination.y + illumination.z;
    finalIllumination *= 0.33 * 100;
    PROFILER_STOP;
    m_player->setIllumination(finalIllumination);
}

void CharacterHandler::updateEnemyLineOfSight(Enemy* enemy)
{
    sf::Vector2f pos = enemy->getEyePos();
    if ((enemy->getFacingDir() == AIComp::Direction::left && pos.x > m_player->getComponent<MovementComp>()->transform.pos.x)
        || (enemy->getFacingDir() == AIComp::Direction::right && pos.x <= m_player->getComponent<MovementComp>()->transform.pos.x)
        || enemy->getState() == AIComp::State::chasing)
    {
        bool playerHidden = false;
        sf::Vector2f dir = m_player->getComponent<ColliderComp>()->getCenterPos() - pos;
        float distance = length(dir);
        normalize(dir);

        if (enemy->getSightRadius() < distance)
            playerHidden = true;

        else if (enemy->getVisionRatingAt(distance) + m_player->getIllumination() < 100)
            playerHidden = true;

        for (size_t i = 0; i < m_occluders->size() && !playerHidden; i++)
        {
            float t = findIntersectionPoint(pos, dir, m_occluders->at(i).p1, m_occluders->at(i).p2);
            if (abs(t + 1) > EPSYLONE && t < distance)
            {
                playerHidden = true;
            }
        }

        if (!playerHidden)
        {
            enemy->notifyEnemy(m_player->getComponent<MovementComp>()->transform.pos + (m_player->getComponent<ColliderComp>()->getSize() / 2.f));
        }
    }
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*m_player, states);

    if (m_boss)
        target.draw(*m_boss, states);


    for (Enemy* enemy : m_enemies)
        target.draw(*enemy, states);
}

void CharacterHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_drawHitboxes)
    {
        target.draw(*m_player->getComponent<ColliderComp>(), states);

        for (Enemy* enemy : m_enemies)
            target.draw(*enemy->getColliderComp(), states);

        if (m_boss)
            target.draw(*m_boss->getColliderComp(), states);
    }

    if (m_drawSightlines)
    {
        sf::VertexArray arr(sf::PrimitiveType::Lines);
        for (Enemy* enemy : m_enemies)
        {
            sf::Color color;
            sf::Vertex v;
            switch (enemy->getState())
            {
            case AIComp::State::chasing:
            case AIComp::State::attacking:
                color = sf::Color::Red;
                break;
            default:
                color = sf::Color::Blue;
                break;
            }
            v.color = color;
            v.position = enemy->getEyePos();
            arr.append(v);

            v.position = enemy->getLastKnownPos();
            arr.append(v);

            sf::CircleShape sightradius;
            sightradius.setFillColor(sf::Color::Transparent);
            sightradius.setOutlineColor(color);
            sightradius.setOutlineThickness(1);

            sightradius.setRadius(enemy->getSightRadius());
            sightradius.setPosition(enemy->getColliderComp()->getCenterPos() - sf::Vector2f(sightradius.getRadius(), sightradius.getRadius()));

            target.draw(sightradius, states);
        }

        //if (boss)
        //{
        //    sf::Vertex v;

        //    v.color = sf::Color::Red;
        //    v.position = boss->getEyePos();
        //    arr.append(v);

        //    v.position = boss->getLastKnownPos();
        //    arr.append(v);
        //}

        //target.draw(arr, states);



    }
}
