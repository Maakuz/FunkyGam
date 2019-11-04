#include "ItemHandler.h"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"

std::vector<Throwable> ItemHandler::throwables;
std::vector<Item*> ItemHandler::itemTemplates;
std::unordered_set<int> ItemHandler::foundItems;


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

    while (!file.eof())
    {
        std::string type;
        file >> type;

        if (type == "[Throwable]")
            loadThrowables(file);

        else if (type == "[Item]")
            loadGatherables(file);
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
            int id = it->item.getID();
            if (!foundItems.count(id))
            {
                ui->displayNewItem(id);
                foundItems.insert(id);
            }

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

void ItemHandler::spawnGatherables(const LevelInfo* level)
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
        pos.y += TILE_SIZE - item.getSize().y;

        item.setPosition(pos);
        Emitter* emitter = nullptr;

        if (item.getEmitterID() != -1)
            emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos);

        gatherItems.push_back(GatherItem{ emitter, item });
    }

    for (sf::Vector2f& point : this->rareGatherPoints)
    {
        int i = rand() % level->rareGatherables.size();

        Item item(*this->itemTemplates[level->rareGatherables[i]]);

        sf::Vector2f pos = point;
        pos.y += TILE_SIZE - item.getSize().y;

        item.setPosition(pos);
        Emitter* emitter = nullptr;

        if (item.getEmitterID() != -1)
            emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos);

        gatherItems.push_back(GatherItem{ emitter, item });
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

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    Throwable* throwable = new Throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(textureID));

    file >> *throwable;

    throwable->setID(itemID);

    itemTemplates.push_back(throwable);
}

void ItemHandler::loadGatherables(std::ifstream& file)
{
    std::string trash;

    int itemID;
    int textureID;

    file >> trash >> itemID;
    file >> trash >> textureID;
    Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(textureID));

    file >> *item;
    item->setID(itemID);
    item->addCollisionComponent(CollisionBox::ColliderKeys::gatherable);

    itemTemplates.push_back(item);
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const Throwable& throwable : this->throwables)
        target.draw(throwable, states);

    for (const GatherItem& item : this->gatherItems)
        target.draw(item.item, states);
}
