#include "ItemHandler.h"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include <sstream>
#include <fstream>

std::vector<const Item*> ItemHandler::itemTemplates;
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
}

ItemHandler::~ItemHandler()
{
    clearTemplates();
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
            loadThrowable(file);

        else if (type == "[Item]")
            loadGatherable(file);

        else if (type == "[Tome]")
            loadTome(file);
    }

    file.close();
}

void ItemHandler::update(float dt, Player* player)
{
    GatherItem* inRange = nullptr;
    for (int i = 0; i < gatherItems.size(); i++)
    {
        GatherItem* item = &gatherItems[i];
        if (item->item.isObtained())
        {
            int id = item->item.getID();
            if (!foundItems.count(id))
            {
                ui->displayNewItem(id);
                foundItems.insert(id);
            }

            if (item->emitter != nullptr)
                item->emitter->kill();

            if (item->count == -1)
                oneTimeItemList.emplace(item->item.getName());

            unordered_erase(gatherItems, gatherItems.begin() + i--);
        }
        else
        {
            if (length(player->getComponent<ColliderComp>()->getCenterPos() - item->item.getTextureCenterPos()) < this->gatherRange)
                inRange = item;
        }
    }

    player->setGatherableInRange(inRange);
}

void ItemHandler::spawnGatherables(const LevelInfo* level, std::vector<CustomHitbox> shrines)
{
    for (GatherItem& item : gatherItems)
    {
        if (item.emitter)
            item.emitter->kill();
    }
    gatherItems.clear();

    for (sf::Vector2f& point : this->gatherPoints)
    {
        int i = rand() % level->gatherables.size();

        Item item(*this->itemTemplates[level->gatherables[i]]);

        sf::Vector2f pos = point;
        pos.y += TILE_SIZE - item.getTextureSize().y;

        item.setPosition(pos);
        Emitter* emitter = nullptr;

        if (item.getEmitterID() != -1)
            emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos + (item.getTextureSize() / 2.f));

        gatherItems.push_back(GatherItem{ emitter, item, 1});
    }

    for (sf::Vector2f& point : this->rareGatherPoints)
    {
        int i = rand() % level->rareGatherables.size();

        Item item(*this->itemTemplates[level->rareGatherables[i]]);

        sf::Vector2f pos = point;
        pos.y += TILE_SIZE - item.getTextureSize().y;

        item.setPosition(pos);
        Emitter* emitter = nullptr;

        if (item.getEmitterID() != -1)
            emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos);

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
            for (const Item* item : this->itemTemplates)
            {
                if (item->getName() == itemName)
                {
                    Item newItem(*item);

                    sf::Vector2f pos = sf::Vector2f(box.min);
                    pos.y += TILE_SIZE - newItem.getTextureSize().y;
                    pos.x += TILE_SIZE / 2 - newItem.getTextureSize().x / 2.f;

                    newItem.setPosition(pos);
                    Emitter* emitter = nullptr;

                    if (newItem.getEmitterID() != -1)
                        emitter = ParticleHandler::addEmitter(newItem.getEmitterID(), pos);

                    gatherItems.push_back({ emitter, newItem, -1});
                }
            }
            
        }

    }
}

const Item* ItemHandler::getTemplate(int itemID)
{
    return itemTemplates[itemID];
}

void ItemHandler::loadThrowable(std::ifstream& file)
{
    std::string trash;

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    Throwable* throwable = new Throwable(sf::Vector2f(), TextureHandler::get().getTexture(textureID), (sf::Vector2f)TextureHandler::get().getTexture(textureID)->getSize());

    file >> *throwable;

    throwable->setID(itemID);

    itemTemplates.push_back(throwable);
}

void ItemHandler::loadGatherable(std::ifstream& file)
{
    std::string trash;

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

    file >> *item;
    item->setID(itemID);

    itemTemplates.push_back(item);
}

void ItemHandler::loadTome(std::ifstream& file)
{
    std::string trash;

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    Tome* item = new Tome(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

    file >> *item;
    item->setID(itemID);
    itemTemplates.push_back(item);
}

void ItemHandler::clearTemplates()
{
    for (const Item* item : itemTemplates)
        delete item;

    itemTemplates.clear();
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const GatherItem& item : this->gatherItems)
        target.draw(item.item, states);
}

bool ItemHandler::isOneTimeItemPickedUp(std::string item)
{
    return oneTimeItemList.count(item);
}

void ItemHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{

}