#pragma once
#include <vector>
#include <unordered_set>
#include <fstream>
#include "Game/Item/Projectile/Throwables/Throwable.h"
#include "Game/Level/Level.h"
#include "Game/Entities/Player.h"
#include "Game/Particles/Emitter.h"
#include "Game/Interface/UIHandler.h"
#include "Renderer/DebugDrawable.h"
#include "Misc/Structs.h"
#include "Game/Handlers/TextureHandler.h"
#include "GatherItem.h"
#include "Consumable.h"
#include "Game/Item/Projectile/Spell/Fireball.h"
#include "Game/Item/Projectile/Spell/Tome.h"

class ItemHandler : public sf::Drawable, public DebugDrawable
{
public:
    ItemHandler(UIHandler* uiHandler);
    virtual ~ItemHandler();
    ItemHandler(const ItemHandler&) = delete;

    void loadTemplates();

    void update(float dt, Player* player);
    void setGatherPoints(std::vector<sf::Vector2f> gatherPoints, std::vector<sf::Vector2f> rareGatherPoints) { this->gatherPoints = gatherPoints; this->rareGatherPoints = rareGatherPoints; };
    void spawnGatherables(const LevelInfo* level, std::vector<CustomHitbox> shrines);

    static const Entity* getTemplate(int itemID);
    static const std::vector<const Entity*>* getTemplateVec() { return &itemTemplates; };
    static const std::unordered_set<int>* getFoundItems() { return &foundItems; };
    static bool isOneTimeItemPickedUp(std::string item);

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    UIHandler* ui;

    static std::vector<const Entity*> itemTemplates;
    static std::unordered_set<int> foundItems;
    static std::unordered_set<std::string> oneTimeItemList;

    std::vector<sf::Vector2f> gatherPoints;
    std::vector<sf::Vector2f> rareGatherPoints;

    std::vector<GatherItem> gatherItems;
    int gatherRange;
    
    template <typename ItemType>
    void load(std::ifstream& file);

    void spawnShrines(std::vector<CustomHitbox> shrines);

    void clearTemplates();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

template<typename ItemType>
inline void ItemHandler::load(std::ifstream& file)
{
    std::string trash;

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    ItemType* item = new ItemType(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

    file >> *item;

    item->getComponent<LogisticsComp>()->id = itemID;

    itemTemplates.push_back(item);
}
