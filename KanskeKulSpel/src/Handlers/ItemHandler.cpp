#include "ItemHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/UnorderedErase.h"
#include "Misc/ConsoleWindow.h"
#include "Misc/Definitions.h"

std::vector<Throwable> ItemHandler::throwables;
std::vector<Item*> ItemHandler::itemTemplates;

ItemHandler::ItemHandler()
{
    ConsoleWindow::get().addCommand("reloadItems", [&](Arguments args)->std::string 
        {
            loadTemplates();

            return "Items reloaded";
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

        itemTemplates.push_back(item);
    }
    file.close();

}

void ItemHandler::update(float dt)
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
}

void ItemHandler::queueColliders()
{
    for (auto& proj : throwables)
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
    for (auto const & throwable : this->throwables)
        target.draw(throwable, states);
}
