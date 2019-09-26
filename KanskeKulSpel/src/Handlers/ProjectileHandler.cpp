#include "ProjectileHandler.h"

std::vector<Throwable> ProjectileHandler::throwables;

ProjectileHandler::ProjectileHandler()
{
}

void ProjectileHandler::update(float dt)
{
    for (auto & throwable : this->throwables)
        throwable.update(dt);
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
