#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"
#include "Entities/Enemies/Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"

#define ENEMY_PATH "src/Entities/Enemies/"



CharacterHandler::CharacterHandler()
{
    player = nullptr;
    occluders = nullptr;

    ConsoleWindow::get().addCommand("resetEnemies", [&](Arguments args)->std::string {
        enemies.clear();
        spawnEnemies();

        return "Enemies has been reset.";
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

void CharacterHandler::initialize(const std::vector<Line>* occluders, UIHandler* uiHandler)
{
    this->occluders = occluders;
    std::vector< Player::Animation> anim;
    anim.push_back(Player::Animation(sf::Vector2u(0, 0), sf::Vector2u(5, 0), 150, sf::Vector2u(1, 0)));
    
    Player::AnimationData playerData(TextureHandler::get().getTexture(TextureHandler::characters::player), 
        sf::Vector2u(6, 1), anim);
    this->player = new Player(playerData, uiHandler, sf::Vector2f(0, 0));

    for (int i = 0; i < ENEMY_TEMPLATE_COUNT; i++)
    {
        std::ifstream file(ENEMY_PATH + ENEMIES[i]);
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

            file >> trash;
            file >> trash >> start.x >> start.y;
            file >> trash >> stop.x >> stop.y;
            file >> trash >> idle.x >> idle.y;
            file >> trash >> speed;

            animations.push_back(MovingEntity::Animation(start, stop, speed, idle));
        }

        MovingEntity::AnimationData data(TextureHandler::get().getTexture(TextureHandler::characters(textureID)), frameCount, animations);

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
    
    for (Enemy* enemy : enemies)
    {
        if (enemy->getState() == Enemy::State::idle || enemy->getState() == Enemy::State::chasing)
            this->updateEnemyLineOfSight(enemy);

        Grunt* g = dynamic_cast<Grunt*>(enemy);
        if (g)
            g->update(dt);
    }
}

void CharacterHandler::queueColliders()
{
    CollisionHandler::queueCollider(this->player);

    for (Enemy* enemy : enemies)
        CollisionHandler::queueCollider(enemy);
}

void CharacterHandler::drawCollision(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(player->getCollisionBox(), states);

    for (Enemy* enemy : enemies)
        target.draw(enemy->getCollisionBox(), states);
}

void CharacterHandler::drawSightLines(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::VertexArray arr(sf::PrimitiveType::Lines);
    for (Enemy* enemy : enemies)
    {
        sf::Vertex v;
        switch (enemy->getState())
        {
        case Enemy::State::chasing:
        case Enemy::State::attacking:
            v.color = sf::Color::Red;
            break;
        default:
            v.color = sf::Color::Blue;
            break;
        }

        v.position = enemy->getEyePos();
        arr.append(v);

        v.position = enemy->getLastKnownPos();
        arr.append(v);
    }

    target.draw(arr, states);
}

void CharacterHandler::updateEnemyLineOfSight(Enemy* enemy)
{
    sf::Vector2f pos = enemy->getEyePos();
    if (    (enemy->getFacingDir() == Enemy::Direction::left  && pos.x >  player->getPosition().x)
        ||  (enemy->getFacingDir() == Enemy::Direction::right && pos.x <= player->getPosition().x)
        ||  enemy->getState()      == Enemy::State::chasing)
    {
        bool playerHidden = false;
        sf::Vector2f dir = this->player->getCenterPos() - pos;
        float distance = lengthSquared(dir);
        normalize(dir);
        for (size_t i = 0; i < occluders->size() && !playerHidden; i++)
        {
            float t = findIntersectionPoint(pos, dir, occluders->at(i).p1, occluders->at(i).p2);
            if (abs(t + 1) > EPSYLONE && t * t < distance)
            {
                playerHidden = true;
            }
        }

        if (!playerHidden)
            enemy->notifyEnemy(player->getPosition() + (player->getSize() / 2.f));
    }
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    for (Enemy* enemy : enemies)
        target.draw(*enemy, states);
}
