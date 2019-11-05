#pragma once
#include <vector>
#include <unordered_set>
#include "Game/Item/Throwables/Throwable.h"
#include "Game/Level/Level.h"
#include "Game/Entities/Player.h"
#include "Game/Particles/Emitter.h"
#include "Game/Interface/UIHandler.h"

class ItemHandler : public sf::Drawable
{
public:
    ItemHandler(UIHandler* uiHandler);
    ~ItemHandler();
    ItemHandler(const ItemHandler&) = delete;

    void loadTemplates();

    void update(float dt, Player* player);
    void setGatherPoints(std::vector<sf::Vector2f> gatherPoints, std::vector<sf::Vector2f> rareGatherPoints) { this->gatherPoints = gatherPoints; this->rareGatherPoints = rareGatherPoints; };
    void spawnGatherables(const LevelInfo* level);
    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static const Item* getTemplate(int itemID);
    static const std::vector<const Item*>* getTemplateVec() { return &itemTemplates; };
    static const std::unordered_set<int>* getFoundItems() { return &foundItems; };
private:
    UIHandler* ui;
    static std::vector<Throwable> throwables;
    static std::vector<const Item*> itemTemplates;
    static std::unordered_set<int> foundItems;
    std::vector<sf::Vector2f> gatherPoints;
    std::vector<sf::Vector2f> rareGatherPoints;

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