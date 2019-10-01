#include "ProjectileHandler.h"
#include "Collision/CollisionHandler.h"
#include "Misc/UnorderedErase.h"

std::vector<Throwable> ProjectileHandler::throwables;

ProjectileHandler::ProjectileHandler()
{
}

void ProjectileHandler::update(float dt)
{
    for (int i = 0; i < throwables.size(); i++)
    {
        if (throwables[i].hasImpacted())
        {
            //throwables.erase(throwables.begin() + i--);
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

void ProjectileHandler::addThrowable(TextureHandler::throwables type, sf::Vector2f pos, sf::Vector2f momentum)
{
    throwables.push_back(Throwable(momentum, pos, TextureHandler::get().getTexture(type)));
}

void ProjectileHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto const & throwable : this->throwables)
        target.draw(throwable, states);
}
