#include "ProjectileHandler.h"
#include "Collision/CollisionHandler.h"

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
            throwables.erase(throwables.begin() + i--);
        }
        else
        {
            throwables[i].update(dt);
            CollisionHandler::queueCollider(&throwables[i]);
        }
    }
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
