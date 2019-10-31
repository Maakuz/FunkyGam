#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"
#include "Entities/Enemies/Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include "Misc/UnorderedErase.h"
#include "Lighting/LightQueue.h"
#include "Misc/Profiler.h"
#include "Misc/Definitions.h"

CharacterHandler::CharacterHandler(UIHandler* uiHandler)
{
    player = nullptr;
    occluders = nullptr;
    this->drawHitboxes = false;
    this->drawSightlines = false;

    this->ui = uiHandler;

    loadPlayer();
    loadEnemies();

    ConsoleWindow::get().addCommand("charResetEnemies", [&](Arguments args)->std::string {
        enemies.clear();
        spawnEnemies();

        return "Enemies has been reset.";
        });

    ConsoleWindow::get().addCommand("charReloadEnemies", [&](Arguments args)->std::string {
        loadEnemies();
        spawnEnemies();

        return "Enemies has been reloaded.";
        });

    ConsoleWindow::get().addCommand("charShowHitboxes", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawHitboxes = std::stoi(args[0]);


            return "Hitboxes on mby";
        });

    ConsoleWindow::get().addCommand("charShowSightlines", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawSightlines = std::stoi(args[0]);


            return "Hitboxes on mby";
        });
}

CharacterHandler::~CharacterHandler()
{
    delete player;

    for (Enemy* enemy : enemies)
        delete enemy;

    for (Enemy* enemy : enemyTemplates)
        delete enemy;
}

void CharacterHandler::initializeLevel(const std::vector<Line>* occluders, sf::Vector2f playerSpawnPoint)
{
    this->occluders = occluders;
    this->player->reset(playerSpawnPoint - this->player->getSize());
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
    std::vector<MovingEntity::Animation> animations;

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

        animations.push_back(MovingEntity::Animation(start, stop, speed, idle, startIdle));
    }

    MovingEntity::AnimationData data(TextureHandler::get().getTexture(textureID), frameCount, animations);


    this->player = new Player(data, ui, sf::Vector2f(0, 0));

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
        std::vector<MovingEntity::Animation> animations;

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

            animations.push_back(MovingEntity::Animation(start, stop, speed, idle, startIdle));
        }

        MovingEntity::AnimationData data(TextureHandler::get().getTexture(textureID), frameCount, animations);

        sf::Vector2f size;
        sf::Vector2f offset;

        file >> trash;
        file >> trash >> size.x >> size.y;
        file >> trash >> offset.x >> offset.y;

        Grunt* grunt = new Grunt(data, sf::Vector2f(0, 0));
        grunt->setSize(size);
        grunt->setEyeLevel(sf::Vector2f(size.x * 0.5f, size.y * 0.2f));
        grunt->moveSpriteOffset(offset);

        file >> *grunt;

        file.close();
        enemyTemplates.push_back(grunt);
    }
}

void CharacterHandler::spawnEnemies()
{
    for (Enemy* enemy : enemies)
        delete enemy;

    enemies.clear();

    //for (const sf::Vector2f& point : spawnPoints)
    //{
        Grunt* grunt = new Grunt(*(Grunt*)enemyTemplates[enemy::grunt]);
        grunt->spawn(spawnPoints[0] - sf::Vector2f(0, grunt->getSize().y));
        enemies.push_back(grunt);
    //}
}

void CharacterHandler::update(float dt, sf::Vector2f mousePos)
{
    this->player->update(dt, mousePos);

    for (auto it = enemies.begin(); it < enemies.end(); it++)
    {
        if ((*it)->isAlive())
        {
            if ((*it)->getState() == Enemy::State::idle || 
                (*it)->getState() == Enemy::State::chasing ||
                (*it)->getState() == Enemy::State::searching ||
                (*it)->getState() == Enemy::State::returning)
                this->updateEnemyLineOfSight((*it));

            Grunt* g = dynamic_cast<Grunt*>((*it));
            if (g)
                g->update(dt);
        }

        else
        {
            delete *it;
            unordered_erase(enemies, it--);
        }
    }
}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(this->player);

    for (Enemy* enemy : enemies)
        CollisionHandler::queueCollider(enemy);
}

//Shadows are not considered at all and that might be a good thing
void CharacterHandler::calculatePlayerIllumination()
{
    /*PROFILER_START("Copy"); //Into the hall of shame with you
    sf::Image image = illuminationTexture->copyToImage();
    sf::Color lightLevel = image.getPixel(0, 0);
    float oldIllumination = lightLevel.r + lightLevel.g + lightLevel.b;
    oldIllumination *= 0.33f / 2.55f;
    PROFILER_STOP*/;

    PROFILER_START("Calculate");
    sf::Vector3f illumination(0, 0, 0);
    for (int i = 0; i < LightQueue::get().getQueue().size(); i++)
    {
        Light* light = LightQueue::get().getQueue()[i];
        bool playerOccluded = false;
        sf::Vector2f dir = this->player->getCenterPos() - light->pos;
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

void CharacterHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (drawHitboxes)
    {
        target.draw(player->getCollisionBox(), states);

        for (Enemy* enemy : enemies)
            target.draw(enemy->getCollisionBox(), states);
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
            case Enemy::State::chasing:
            case Enemy::State::attacking:
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
            sightradius.setPosition(enemy->getCenterPos() - sf::Vector2f(sightradius.getRadius(), sightradius.getRadius()));

            target.draw(sightradius, states);
        }

        target.draw(arr, states);

    }
}

void CharacterHandler::updateEnemyLineOfSight(Enemy* enemy)
{
    sf::Vector2f pos = enemy->getEyePos();
    if ((enemy->getFacingDir() == Enemy::Direction::left && pos.x > player->getPosition().x)
        || (enemy->getFacingDir() == Enemy::Direction::right && pos.x <= player->getPosition().x)
        || enemy->getState() == Enemy::State::chasing)
    {
        bool playerHidden = false;
        sf::Vector2f dir = this->player->getCenterPos() - pos;
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
            enemy->notifyEnemy(player->getPosition() + (player->getSize() / 2.f));
        }
    }
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    for (Enemy* enemy : enemies)
        target.draw(*enemy, states);
}
