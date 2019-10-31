#include "ItemHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Misc/Definitions.h"
#include "ParticleHandler.h"
#include "Misc/VectorFunctions.h"

#define LEVEL1_ITEM_COUNT 2
#define LEVEL1_RARE_ITEM_COUNT 1

const int LEVEL1_ITEMS[LEVEL1_ITEM_COUNT] = { 4, 5 };
const int LEVEL1_RARE_ITEMS[LEVEL1_RARE_ITEM_COUNT] = {6};

std::vector<Throwable> ItemHandler::throwables;
std::vector<Item*> ItemHandler::itemTemplates;


ItemHandler::ItemHandler()
{
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
    for (Item* item : itemTemplates)
        delete item;
}

void ItemHandler::loadTemplates()
{
    for (Item* item : itemTemplates)
        delete item;
    itemTemplates.clear();

    std::ifstream file(DATA_PATH "Items.mop");
    if (!file.is_open())
    {
        system("Pause");
        exit(-44);
    }

    loadThrowables(file);
    loadGatherables(file);


    file.close();

}

void ItemHandler::update(float dt, Player* player)
{
    for (int i = 0; i < throwables.size(); i++)
    {
        if (throwables[i].hasDetonated())
        {
            CollisionHandler::queueExplosion(throwables[i].getExplosion());
            unordered_erase(throwables, throwables.begin() + i--);
        }
        else
        {
            throwables[i].update(dt);
        }
    }

    Item* inRange = nullptr;
    auto it = gatherItems.begin();
    while (it != gatherItems.end())
    {
        if (it->item.isObtained())
        {
            
            if (it->emitter != nullptr)
                it->emitter->kill();

            unordered_erase(gatherItems, it);
        }
        else
        {
            if (length(player->getCenterPos() - it->item.getCenterPos()) < this->gatherRange)
                inRange = &it->item;

            it++;
        }
    }

    player->setGatherableInRange(inRange);
}

void ItemHandler::spawnGatherables(Level level)
{
    for (GatherItem& item : gatherItems)
    {
        if (item.emitter)
            item.emitter->kill();
    }
    gatherItems.clear();

    if (level == Level::forest)
    {
        for (sf::Vector2f& point : this->gatherPoints)
        {
            int i = rand() % LEVEL1_ITEM_COUNT;

            Item item(*this->itemTemplates[LEVEL1_ITEMS[i]]);

            sf::Vector2f pos = point;
            pos.y += TILE_SIZE - item.getSize().y;

            item.setPosition(pos);
            Emitter* emitter = nullptr;

            if (item.getEmitterID() != -1)
                emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos);

            gatherItems.push_back(GatherItem{emitter, item});
        }

        for (sf::Vector2f& point : this->rareGatherPoints)
        {
            int i = rand() % LEVEL1_RARE_ITEM_COUNT;

            Item item(*this->itemTemplates[LEVEL1_RARE_ITEMS[i]]);

            sf::Vector2f pos = point;
            pos.y += TILE_SIZE - item.getSize().y;

            item.setPosition(pos);
            Emitter* emitter = nullptr;

            if (item.getEmitterID() != -1)
                emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos);

            gatherItems.push_back(GatherItem{ emitter, item });
        }
    }
}

void ItemHandler::queueColliders()
{
    for (Throwable& proj : this->throwables)
        CollisionHandler::queueCollider(&proj);
}

void ItemHandler::addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower)
{
    Throwable item(*dynamic_cast<Throwable*>(itemTemplates[id]));
    item.throwItem(pos, momentum, thrower);
    throwables.push_back(item);
}

const Item* ItemHandler::getTemplate(int itemID)
{
    return itemTemplates[itemID];
}

void ItemHandler::loadThrowables(std::ifstream& file)
{
    std::string trash;
    int count;

    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int itemID;
        int textureID;

        file >> trash;
        file >> trash >> itemID;
        file >> trash >> textureID;
        Throwable* throwable = new Throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(textureID));

        file >> *throwable;

        throwable->setID(itemID);

        itemTemplates.push_back(throwable);
    }
}

void ItemHandler::loadGatherables(std::ifstream& file)
{
    std::string trash;
    int count;
    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int itemID;
        int textureID;

        file >> trash;
        file >> trash >> itemID;
        file >> trash >> textureID;
        Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

        file >> *item;
        item->setID(itemID);
        item->addCollisionComponent(CollisionBox::ColliderKeys::gatherable);

        itemTemplates.push_back(item);
    }
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const Throwable& throwable : this->throwables)
        target.draw(throwable, states);

    for (const GatherItem& item : this->gatherItems)
        target.draw(item.item, states);
}
