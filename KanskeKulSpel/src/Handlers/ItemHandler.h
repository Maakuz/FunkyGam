#pragma once
#include <vector>
#include "Entities/Throwables/Throwable.h"

class ItemHandler : public sf::Drawable
{
public:
    ItemHandler();
    ~ItemHandler();
    ItemHandler(const ItemHandler&) = delete;

    void loadTemplates();

    void update(float dt);
    void setGatherPoints(std::vector<sf::Vector2f> gatherPoints) { this->gatherPoints = gatherPoints; };

    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static const Item* getTemplate(int itemID);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    static std::vector<Throwable> throwables;
    static std::vector<Item*> itemTemplates;
    std::vector<Item> gatherItems;
    std::vector<sf::Vector2f> gatherPoints;
};