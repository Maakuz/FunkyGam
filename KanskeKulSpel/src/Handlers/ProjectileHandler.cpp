#include "ProjectileHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/UnorderedErase.h"

std::vector<Throwable> ProjectileHandler::throwables;
std::vector<Throwable> ProjectileHandler::throwableTemplates;

ProjectileHandler::ProjectileHandler()
{
}

void ProjectileHandler::loadTemplates()
{
    throwableTemplates.clear();

    std::ifstream file("src/Entities/Throwables/Throwables.trw");
    if (!file.is_open())
        exit(-44);

    std::string trash;
    int count;
    
    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int textureID;
        file >> trash;
        file >> trash >> textureID;
        Throwable throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(TextureHandler::throwables(textureID)));
        
        file >> throwable;

        throwable.setID(i);
        
        throwableTemplates.push_back(throwable);
    }

    file.close();

}

void ProjectileHandler::update(float dt)
{
    for (int i = 0; i < throwables.size(); i++)
    {
        if (throwables[i].hasDetonated())
        {
            unordered_erase(throwables, throwables.begin() + i--);
        }
        else
        {
            throwables[i].update(dt);
        }
    }
}

void ProjectileHandler::queueColliders()
{
    for (auto& proj : throwables)
        CollisionHandler::queueCollider(&proj);
}

void ProjectileHandler::addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum)
{
    throwables.push_back(throwableTemplates[id]);
}

void ProjectileHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto const & throwable : this->throwables)
        target.draw(throwable, states);
}
