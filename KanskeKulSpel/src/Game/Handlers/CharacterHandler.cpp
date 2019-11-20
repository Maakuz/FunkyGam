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

CharacterHandler::CharacterHandler(UIHandler* uiHandler)
{
    player = nullptr;
    occluders = nullptr;
    this->drawHitboxes = false;
    this->drawSightlines = false;
    this->boss = nullptr;
    this->bossSpawner = nullptr;

    this->ui = uiHandler;

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

            drawHitboxes = std::stoi(args[0]);


            return "Hitboxes on mby";
        });

    ConsoleWindow::get().addCommand("charSightlines", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawSightlines = std::stoi(args[0]);


            return "Hitboxes on mby";
        });
}

CharacterHandler::~CharacterHandler()
{
    if (player)
        delete player;

    if (boss)
        delete boss;

    for (Enemy* enemy : enemies)
        delete enemy;

    for (Enemy* enemy : enemyTemplates)
        delete enemy;
}

void CharacterHandler::initializeLevel(const std::vector<Line>* occluders, sf::Vector2f playerSpawnPoint)
{
    this->bossSpawner = nullptr;
    if (boss)
    {
        delete boss;
        boss = nullptr;
    }

    this->occluders = occluders;
    this->player->reset(playerSpawnPoint - this->player->getCollider().getSize());
}

void CharacterHandler::loadPlayer()
{
    if (player)
        delete player;

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


    this->player = new Player(data, ui, sf::Vector2f(0, 0), sf::Vector2f(32, 92));

    file >> *this->player;


    file.close();
}

void CharacterHandler::loadEnemies()
{
    for (Enemy* enemy : enemyTemplates)
        delete enemy;

    enemyTemplates.clear();

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
        enemyTemplates.push_back(enemy);
    }

    //Bosses get the same treatment
    for (Boss* boss : bossTemplates)
        delete boss;

    bossTemplates.clear();

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
        bossTemplates.push_back(boss);
    }
}

void CharacterHandler::spawnEnemies(const LevelInfo* info)
{
    for (Enemy* enemy : enemies)
        delete enemy;

    enemies.clear();

    for (const sf::Vector2f& point : spawnPoints)
    {
        int i = rand() % info->enemies.size();
        
        Enemy* enemy = spawnEnemy(info->enemies[i]);
        enemy->spawn(point - sf::Vector2f(0, enemy->getCollider().getSize().y));
        enemies.push_back(enemy);
    }
}

void CharacterHandler::update(float dt, sf::Vector2f mousePos)
{
    if (!boss && bossSpawner)
    {
        bool spawn = true;
        if (!this->bossSpawner->playerPosCriteria.contains(this->player->getMovementComp().transform.pos))
            spawn = false;

        if (spawn)
        {
            for (const std::string& str : this->bossSpawner->playerItemCriteria)
            {
                if (!ItemHandler::isOneTimeItemPickedUp(str))
                    spawn = false;
            }
        }

        if (spawn)
            spawnBoss((BossType)bossSpawner->bossID, bossSpawner->pos);
    }

    if (boss)
    {
        boss->update(dt, this->player->getMovementComp().transform.pos);
    }

    this->player->update(dt, mousePos);
    
    for (int i = 0; i < enemies.size(); i++)
    {
        Enemy* enemy = enemies[i];
        if (enemy->isAlive())
        {
            if (enemy->getState() == AIComp::State::idle || 
                enemy->getState() == AIComp::State::chasing ||
                enemy->getState() == AIComp::State::searching ||
                enemy->getState() == AIComp::State::returning)
                this->updateEnemyLineOfSight(enemy);

            if (enemy->isHealthChanged())
                ui->displayEnemyDamage(enemy->getHealthPercentage());

            enemy->update(dt);
        }

        else
        {
            delete enemy;
            unordered_erase(enemies, enemies.begin() + i--);
            ui->hideEnemyDamage();
        }
    }
}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(this->player);

    for (Enemy* enemy : enemies)
        CollisionHandler::queueCollider(enemy);

    if (boss)
        CollisionHandler::queueCollider(boss);
}

void CharacterHandler::setBossSpawner(const BossSpawner* bossSpawner) 
{ 
    this->bossSpawner = bossSpawner;
}

void CharacterHandler::spawnBoss(BossType bossType, sf::Vector2f pos)
{
    if (boss)
        delete boss;

    switch (bossType)
    {
    case BossType::fishmonger:
        boss = new FishMonger(*dynamic_cast<FishMonger*>(bossTemplates[BossType::fishmonger]));
        break;
    default:
        boss = new FishMonger(*dynamic_cast<FishMonger*>(bossTemplates[BossType::fishmonger]));
        break;
    }

    boss->spawn(pos - sf::Vector2f(0, boss->getCollider().getSize().y));
}

Enemy* CharacterHandler::spawnEnemy(int enemyType)
{
    Enemy* enemy;
    switch (enemyType)
    {
    case EnemyType::grunt:
        enemy = new Grunt(*dynamic_cast<Grunt*>(enemyTemplates[EnemyType::grunt]));
        break;
    case EnemyType::bird:
        enemy = new Bird(*dynamic_cast<Bird*>(enemyTemplates[EnemyType::bird]));
        break;
    default:
        enemy = new Grunt(*dynamic_cast<Grunt*>(enemyTemplates[EnemyType::grunt]));
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
        sf::Vector2f dir = this->player->getCollider().getCenterPos() - light->pos;
        float distance = length(dir);
        normalize(dir);
        if (light->radius < distance)
            playerOccluded = true;

        for (size_t j = 0; j < occluders->size() && !playerOccluded; j++)
        {
            float t = findIntersectionPoint(light->pos, dir, occluders->at(j).p1, occluders->at(j).p2);
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
    player->setIllumination(finalIllumination);
}

void CharacterHandler::updateEnemyLineOfSight(Enemy* enemy)
{
    sf::Vector2f pos = enemy->getEyePos();
    if ((enemy->getFacingDir() == AIComp::Direction::left && pos.x > player->getMovementComp().transform.pos.x)
        || (enemy->getFacingDir() == AIComp::Direction::right && pos.x <= player->getMovementComp().transform.pos.x)
        || enemy->getState() == AIComp::State::chasing)
    {
        bool playerHidden = false;
        sf::Vector2f dir = this->player->getCollider().getCenterPos() - pos;
        float distance = length(dir);
        normalize(dir);

        if (enemy->getSightRadius() < distance)
            playerHidden = true;

        else if (enemy->getVisionRatingAt(distance) + player->getIllumination() < 100)
            playerHidden = true;

        for (size_t i = 0; i < occluders->size() && !playerHidden; i++)
        {
            float t = findIntersectionPoint(pos, dir, occluders->at(i).p1, occluders->at(i).p2);
            if (abs(t + 1) > EPSYLONE && t < distance)
            {
                playerHidden = true;
            }
        }

        if (!playerHidden)
        {
            enemy->notifyEnemy(player->getMovementComp().transform.pos + (player->getCollider().getSize() / 2.f));
        }
    }
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    if (boss)
        target.draw(*boss, states);


    for (Enemy* enemy : enemies)
        target.draw(*enemy, states);
}

void CharacterHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (drawHitboxes)
    {
        target.draw(player->getCollider(), states);

        for (Enemy* enemy : enemies)
            target.draw(enemy->getCollider(), states);

        if (boss)
            target.draw(boss->getCollider(), states);
    }

    if (drawSightlines)
    {
        sf::VertexArray arr(sf::PrimitiveType::Lines);
        for (Enemy* enemy : enemies)
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
            sightradius.setPosition(enemy->getCollider().getCenterPos() - sf::Vector2f(sightradius.getRadius(), sightradius.getRadius()));

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
