#pragma once
#include <vector>
#include "Entities/Items/Throwables/Throwable.h"
#include "Level/Levels.h"
#include "Entities/Player.h"
#include "Particles/Emitter.h"

class ItemHandler : public sf::Drawable
{
public:
    ItemHandler();
    ~ItemHandler();
    ItemHandler(const ItemHandler&) = delete;

    void loadTemplates();

    void update(float dt, Player* player);
    void setGatherPoints(std::vector<sf::Vector2f> gatherPoints) { this->gatherPoints = gatherPoints; };
    void spawnGatherables(Level level);
    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static const Item* getTemplate(int itemID);

private:
    static std::vector<Throwable> throwables;
    static std::vector<Item*> itemTemplates;
    std::vector<sf::Vector2f> gatherPoints;

    struct GatherItem
    {
        Emitter* emitter;
        Item item;
    };
    std::vector<GatherItem> gatherItems;
    int gatherRange;
    
    void loadThrowables(std::ifstream& file);
    void loadGatherables(std::ifstream& file);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};