#pragma once
#include <vector>
#include "Entities/Throwables/Throwable.h"

class ItemHandler : public sf::Drawable
{
public:
    ItemHandler();
    ~ItemHandler() {};

    void loadTemplates();

    void update(float dt);

    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static const Throwable& getTemplate(int itemID);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    static std::vector<Throwable> throwables;
    static std::vector<Throwable> throwableTemplates;
};