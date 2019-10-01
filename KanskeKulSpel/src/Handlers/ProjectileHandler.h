#pragma once
#include <vector>
#include "Entities/Throwable.h"
#include "Handlers/TextureHandler.h"

class ProjectileHandler : public sf::Drawable
{
public:
    enum throwType
    {
        bomb = 0
    };

    ProjectileHandler();
    ~ProjectileHandler() {};

    void update(float dt);

    void queueColliders();

    static void addThrowable(TextureHandler::throwables type, sf::Vector2f pos, sf::Vector2f momentum);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    static std::vector<Throwable> throwables;
};