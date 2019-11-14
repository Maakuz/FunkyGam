#include "ItemHandler.h"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include <sstream>

std::vector<Throwable> ItemHandler::throwables;
std::vector<Spell*> ItemHandler::spells;
std::vector<const Item*> ItemHandler::itemTemplates;
std::unordered_map<std::string, const Spell*> ItemHandler::spellTemplates;
std::unordered_set<int> ItemHandler::foundItems;


ItemHandler::ItemHandler(UIHandler* uiHandler)
{
    this->ui = uiHandler;
    this->gatherRange = 64;
    this->drawHitboxes = false;
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

    ConsoleWindow::get().addCommand("itemShowHitboxes", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawHitboxes = std::stoi(args[0]);


            return "Hitboxes on mby";
        });
}

ItemHandler::~ItemHandler()
{
    clearTemplates();

    for (Spell* spell : spells)
        delete spell;
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

    file.open(DATA_PATH "Spells.mop");
    if (!file.is_open())
    {
        system("Pause");
        exit(-44);
    }

    while (!file.eof())
    {
        std::string spellType;
        std::string trash;

        file >> spellType;

        if (spellType == "[Fireball]")
        {
            Fireball* spell = new Fireball(sf::Vector2f());
            file >> *spell;
            this->spellTemplates.emplace(spell->getName(), spell);
        }
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

    for (int i = 0; i < spells.size(); i++)
    {
        if (spells[i]->isComplete())
        {
            delete spells[i];
            unordered_erase(spells, spells.begin() + i--);
        }

        else
            spells[i]->update(dt);
    }

    GatherItem* inRange = nullptr;
    auto it = gatherItems.begin();
    while (!gatherItems.empty() && it != gatherItems.end())
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

            if (it->count == -1)
                oneTimeItemList.emplace(it->item.getName());

            unordered_erase(gatherItems, it);
        }
        else
        {
            if (length(player->getCenterPos() - it->item.getCenterPos()) < this->gatherRange)
                inRange = it._Ptr;

            it++;
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
        pos.y += TILE_SIZE - item.getSize().y;

        item.setPosition(pos);
        Emitter* emitter = nullptr;

        if (item.getEmitterID() != -1)
            emitter = ParticleHandler::addEmitter(item.getEmitterID(), pos + (item.getSize() / 2.f));

        gatherItems.push_back(GatherItem{ emitter, item, 1});
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
                    pos.y += TILE_SIZE - newItem.getSize().y;
                    pos.x += TILE_SIZE / 2 - newItem.getSize().x / 2.f;

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

void ItemHandler::queueColliders()
{
    for (Throwable& proj : this->throwables)
        CollisionHandler::queueCollider(&proj);
}

void ItemHandler::addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower)
{
    Throwable item(*dynamic_cast<const Throwable*>(itemTemplates[id]));
    item.throwItem(pos, momentum, thrower);
    throwables.push_back(item);
}

void ItemHandler::addSpell(int tomeID, sf::Vector2f pos, sf::Vector2f destination, float channelTime)
{
    const Tome* tome = dynamic_cast<const Tome*>(itemTemplates[tomeID]);

    if (tome)
    {
        const Fireball* fire = dynamic_cast<const Fireball*>(spellTemplates[tome->getSpell()]);

        if (fire)
        {
            Fireball* newFire = new Fireball(*fire);
            newFire->cast(pos, destination, channelTime);
            spells.push_back(newFire);
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
    Throwable* throwable = new Throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(textureID));

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
    item->addCollisionComponent(Collider::ColliderKeys::gatherable);

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
    item->addCollisionComponent(Collider::ColliderKeys::gatherable);

    itemTemplates.push_back(item);
}

void ItemHandler::clearTemplates()
{
    for (const Item* item : itemTemplates)
        delete item;

    for (auto& pair : spellTemplates)
        delete pair.second;

    itemTemplates.clear();
    spellTemplates.clear();
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (const Throwable& throwable : this->throwables)
        target.draw(throwable, states);

    for (const GatherItem& item : this->gatherItems)
        target.draw(item.item, states);
}

void ItemHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (drawHitboxes)
        for (const Throwable& throwable : this->throwables)
            target.draw(throwable.getCollider(), states);
}