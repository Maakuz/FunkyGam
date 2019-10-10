#include "CharacterHandler.h"
#include "Collision/CollisionHandler.h"
#include "TextureHandler.h"
#include "Entities/Enemies/Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"

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
}

void CharacterHandler::initialize(const std::vector<Line>* occluders)
{
    this->occluders = occluders;
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150, sf::Vector2u(1, 0));
    this->player = new Player(data, sf::Vector2f(0, 0));
    this->player->setAnimationData(data);
}

void CharacterHandler::spawnEnemies()
{
    Player::AnimationData data(TextureHandler::get().getTexture(TextureHandler::misc::playerSprite), sf::Vector2u(6, 1), 150, sf::Vector2u(1, 0));
    Grunt* grunt = new Grunt(data, spawnPoints[0] + sf::Vector2f(0, -50));
    grunt->setColor(sf::Color(255, 50, 50, 255));
    enemies.push_back(grunt);
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
            v.color = sf::Color::Red;
            break;
        default:
            v.color = sf::Color::Blue;
            break;
        }

        v.position = enemy->getEyePos();
        arr.append(v);

        v.position = this->player->getCenterPos();
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
            enemy->notifyEnemy(player->getPosition());
    }
}

void CharacterHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*player, states);

    for (Enemy* enemy : enemies)
        target.draw(*enemy, states);
}
