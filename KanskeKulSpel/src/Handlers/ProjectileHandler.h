#pragma once
#include <vector>
#include "Entities/Throwables/Throwable.h"

class ProjectileHandler : public sf::Drawable
{
public:
    enum throwType
    {
        bomb = 0
    };

    ProjectileHandler();
    ~ProjectileHandler() {};

    void loadTemplates();

    void update(float dt);

    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    static std::vector<Throwable> throwables;
    static std::vector<Throwable> throwableTemplates;
};