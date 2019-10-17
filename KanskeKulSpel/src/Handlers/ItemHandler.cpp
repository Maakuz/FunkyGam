#include "ItemHandler.h"
#include "Collision/CollisionHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/UnorderedErase.h"

std::vector<Throwable> ItemHandler::throwables;
std::vector<Throwable> ItemHandler::throwableTemplates;

ItemHandler::ItemHandler()
{
}

void ItemHandler::loadTemplates()
{
    throwableTemplates.clear();

    std::ifstream file("src/Data/Throwables.mop");
    if (!file.is_open())
        exit(-44);

    std::string trash;
    int count;
    
    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int textureID;
        file >> trash;
        file >> trash >> textureID;
        Throwable throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(textureID));
        
        file >> throwable;

        throwable.setID(i);
        
        throwableTemplates.push_back(throwable);
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
    Throwable item(throwableTemplates[id]);
    item.throwItem(pos, momentum, thrower);
    throwables.push_back(item);
}

const Throwable& ItemHandler::getTemplate(int itemID)
{
    return throwableTemplates[itemID];
}

void ItemHandler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto const & throwable : this->throwables)
        target.draw(throwable, states);
}
