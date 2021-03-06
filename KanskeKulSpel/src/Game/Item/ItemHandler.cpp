#include "ItemHandler.h"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include <sstream>

std::vector<const Entity*> ItemHandler::itemTemplates;
std::unordered_set<int> ItemHandler::foundItems;
std::unordered_set<std::string> ItemHandler::oneTimeItemList;

ItemHandler::ItemHandler(UIHandler* uiHandler)
{
    this->ui = uiHandler;
    this->gatherRange = 64;
    ConsoleWindow::get().addCommand("reloadItems", [&](Arguments args)->std::string 
        {
            loadTemplates();

            return "Items reloaded";
        });

    ConsoleWindow::get().addCommand("setGatherRange", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument int range";

            gatherRange = std::stoi(args[0]);

            return "Gather range is now " + args[0] + "!";
        });

    loadTemplates();
}

ItemHandler::~ItemHandler()
{
    clearTemplates();

    for (GatherItem& gather : gatherItems)
        delete gather.emitter;

    gatherItems.clear();
}

void ItemHandler::loadTemplates()
{
    clearTemplates();

    std::ifstream file(DATA_PATH "Items.mop");
    if (!file.is_open())
    {
        system("Pause");
        exit(-44);
    }

    while (!file.eof())
    {
        std::string type;
        file >> type;

        if (type == "[Throwable]")
            load<Throwable>(file);

        else if (type == "[Item]")
            load<Item>(file);

        else if (type == "[Tome]")
            load<Tome>(file);

        else if (type == "[Consumable]")
            load<Consumable>(file);
    }

    file.close();
}

void ItemHandler::update(float dt, Player* player)
{
    GatherItem* inRange = nullptr;
    for (int i = 0; i < gatherItems.size(); i++)
    {
        GatherItem* item = &gatherItems[i];
        LogisticsComp* logistic = item->item.getComponent<LogisticsComp>();
        if (logistic->isObtained())
        {
            int id = logistic->id;
            if (!foundItems.count(id))
            {
                ui->displayNewItem(id);
                foundItems.insert(id);
            }

            if (item->emitter)
            {
                ParticleHandler::destroyEmitter(item->emitter);
                item->emitter = nullptr;
            }

            if (item->count == -1)
                oneTimeItemList.emplace(logistic->name);

            unordered_erase(gatherItems, gatherItems.begin() + i--);
        }
        else
        {
            if (length(player->getComponent<ColliderComp>()->getCenterPos() - item->item.getComponent<SpriteComp>()->getTextureCenterPos()) < this->gatherRange)
                inRange = item;

            if (item->emitter)
                ParticleHandler::queueEmitter(item->emitter);
        }
    }

    player->setGatherableInRange(inRange);
}

void ItemHandler::spawnGatherables(const LevelInfo* level, std::vector<CustomHitbox> shrines)
{
    for (GatherItem& item : gatherItems)
    {
        if (item.emitter)
            ParticleHandler::destroyEmitter(item.emitter, true);
    }
    gatherItems.clear();

    for (sf::Vector2f& point : this->gatherPoints)
    {
        int i = rand() % level->gatherables.size();

        Entity item(*this->itemTemplates[level->gatherables[i]]);
        SpriteComp* sprite = item.getComponent<SpriteComp>();
        LogisticsComp* logistics = item.getComponent<LogisticsComp>();

        sf::Vector2f pos = point;
        pos.y += TILE_SIZE - sprite->getTexture()->getSize().y;

        sprite->setPosition(pos);
        Emitter* emitter = nullptr;

        if (logistics->emitterID != -1)
            emitter = ParticleHandler::createEmitter(logistics->emitterID, pos + (sf::Vector2f(sprite->getTexture()->getSize()) / 2.f));

        gatherItems.push_back(GatherItem{ emitter, item, 1});
    }

    for (sf::Vector2f& point : this->rareGatherPoints)
    {
        int i = rand() % level->rareGatherables.size();

        Entity item(*this->itemTemplates[level->rareGatherables[i]]);
        SpriteComp* sprite = item.getComponent<SpriteComp>();
        LogisticsComp* logistics = item.getComponent<LogisticsComp>();

        sf::Vector2f pos = point;
        pos.y += TILE_SIZE - sprite->getTexture()->getSize().y;

        sprite->setPosition(pos);
        Emitter* emitter = nullptr;

        if (logistics->emitterID != -1)
            emitter = ParticleHandler::createEmitter(logistics->emitterID, pos + (sf::Vector2f(sprite->getTexture()->getSize()) / 2.f));

        gatherItems.push_back(GatherItem{ emitter, item, 1 });
    }

    this->spawnShrines(shrines);
}

void ItemHandler::spawnShrines(std::vector<CustomHitbox> shrines)
{
    for (CustomHitbox& box : shrines)
    {
        std::stringstream sstream(box.flag);
        std::string itemName;
        sstream >> itemName;
        std::getline(sstream, itemName);
        while (itemName.size() > 0 && itemName[0] == ' ')
            itemName.erase(itemName.begin());

        if (!oneTimeItemList.count(itemName))
        {
            for (const Entity* item : this->itemTemplates)
            {
                if (item->getComponent<LogisticsComp>()->name == itemName)
                {
                    Entity newItem(*item);
                    SpriteComp* sprite = newItem.getComponent<SpriteComp>();
                    LogisticsComp* logistics = newItem.getComponent<LogisticsComp>();

                    sf::Vector2f pos = sf::Vector2f(box.min);
                    pos.y += TILE_SIZE - sprite->getTexture()->getSize().y;
                    pos.x += TILE_SIZE / 2 - sprite->getTexture()->getSize().x / 2.f;

                    sprite->setPosition(pos);
                    Emitter* emitter = nullptr;

                    if (logistics->emitterID != -1)
                        emitter = ParticleHandler::createEmitter(logistics->emitterID, pos + (sf::Vector2f(sprite->getTexture()->getSize()) / 2.f));

                    gatherItems.push_back({ emitter, newItem, -1});
                }
            }
            
        }

    }
}

const Entity* ItemHandler::getTemplate(int itemID)
{
    return itemTemplates[itemID];
}

void ItemHandler::clearTemplates()
{
    for (const Entity* item : itemTemplates)
        delete item;

    itemTemplates.clear();
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const GatherItem& item : this->gatherItems)
        target.draw(*item.item.getComponent<SpriteComp>(), states);
}

bool ItemHandler::isOneTimeItemPickedUp(std::string item)
{
    return oneTimeItemList.count(item);
}

void ItemHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{

}