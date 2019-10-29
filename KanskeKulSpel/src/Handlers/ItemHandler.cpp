#include "ItemHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Misc/Definitions.h"
#include "ParticleHandler.h"
#include "Misc/VectorFunctions.h"

#define LEVEL1_ITEM_COUNT 2

const int LEVEL1_ITEMS[LEVEL1_ITEM_COUNT] = {4, 5};

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
    itemTemplates.clear();

    std::ifstream file(DATA_PATH "Throwables.mop");
    if (!file.is_open())
    {
        system("Pause");
        exit(-44);
    }

    std::string trash;
    int count;
    int id = 0;
    
    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int textureID;
        file >> trash;
        file >> trash >> textureID;
        Throwable* throwable = new Throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(textureID));
        
        file >> *throwable;

        throwable->setID(id++);
        
        itemTemplates.push_back(throwable);
    }

    file.close();

    //load items
    file.open(DATA_PATH "Gatherables.mop");
    if (!file.is_open())
    {
        system("Pause");
        exit(-45);
    }

    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int textureID;

        file >> trash;
        file >> trash >> textureID;
        Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

        file >> *item;
        item->setID(id++);
        item->addCollisionComponent(CollisionBox::ColliderKeys::gatherable);

        itemTemplates.push_back(item);
    }
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

            it = gatherItems.erase(it);
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

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const Throwable& throwable : this->throwables)
        target.draw(throwable, states);

    for (const GatherItem& item : this->gatherItems)
        target.draw(item.item, states);
}
