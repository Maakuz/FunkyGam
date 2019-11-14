#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Game/Item/Throwables/Throwable.h"
#include "Game/Level/Level.h"
#include "Game/Entities/Player.h"
#include "Game/Particles/Emitter.h"
#include "Game/Interface/UIHandler.h"
#include "Renderer/DebugDrawable.h"
#include "Misc/Structs.h"
#include "GatherItem.h"
#include "Game/Item/Spell/Fireball.h"
#include "Game/Item/Spell/Tome.h"

class ItemHandler : public sf::Drawable, public DebugDrawable
{
public:
    ItemHandler(UIHandler* uiHandler);
    ~ItemHandler();
    ItemHandler(const ItemHandler&) = delete;

    void loadTemplates();

    void update(float dt, Player* player);
    void setGatherPoints(std::vector<sf::Vector2f> gatherPoints, std::vector<sf::Vector2f> rareGatherPoints) { this->gatherPoints = gatherPoints; this->rareGatherPoints = rareGatherPoints; };
    void spawnGatherables(const LevelInfo* level, std::vector<CustomHitbox> shrines);
    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static void addSpell(int tomeID, sf::Vector2f pos, sf::Vector2f destination, float channelTime);
    static const Item* getTemplate(int itemID);
    static const std::vector<const Item*>* getTemplateVec() { return &itemTemplates; };
    static const std::unordered_set<int>* getFoundItems() { return &foundItems; };

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    UIHandler* ui;
    static std::vector<Throwable> throwables;
    static std::vector<Spell*> spells;
    static std::vector<const Item*> itemTemplates;
    static std::unordered_map<std::string, const Spell*> spellTemplates;
    static std::unordered_set<int> foundItems;

    std::vector<sf::Vector2f> gatherPoints;
    std::vector<sf::Vector2f> rareGatherPoints;
    std::unordered_set<std::string> oneTimeItemList;

    std::vector<GatherItem> gatherItems;
    int gatherRange;
    
    bool drawHitboxes;

    void loadThrowable(std::ifstream& file);
    void loadGatherable(std::ifstream& file);
    void loadTome(std::ifstream& file);
    void spawnShrines(std::vector<CustomHitbox> shrines);

    void clearTemplates();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};