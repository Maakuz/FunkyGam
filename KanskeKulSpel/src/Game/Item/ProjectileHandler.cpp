#include "ProjectileHandler.h"
#include "Game/Misc/Definitions.h"
#include "Game/Collision/CollisionHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include "Game/Item/ItemHandler.h"
#include "Misc/ConsoleWindow.h"
#include <fstream>

std::vector<Throwable> ProjectileHandler::throwables;
std::vector<Spell*> ProjectileHandler::spells;
std::unordered_map<std::string, const Spell*> ProjectileHandler::spellTemplates;

ProjectileHandler::ProjectileHandler()
{
    this->drawHitboxes = false;

    ConsoleWindow::get().addCommand("itemShowHitboxes", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "Missing argument 0 or 1";

            drawHitboxes = std::stoi(args[0]);


            return "Hitboxes on mby";
        });

    ConsoleWindow::get().addCommand("reloadSpells", [&](Arguments args)->std::string
        {

            loadTemplates();

            return "relod";
        });
}

ProjectileHandler::~ProjectileHandler()
{
    for (Spell* spell : spells)
        delete spell;

    for (auto& pair : spellTemplates)
        delete pair.second;
    spellTemplates.clear();
}

void ProjectileHandler::update(float dt, Player* player)
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

}

void ProjectileHandler::loadTemplates()
{
    for (auto& pair : spellTemplates)
        delete pair.second;
    spellTemplates.clear();

    std::ifstream file(DATA_PATH "Spells.mop");
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

void ProjectileHandler::queueColliders()
{
    for (Throwable& proj : this->throwables)
        CollisionHandler::queueCollider(&proj);
}

void ProjectileHandler::addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Collidable* thrower)
{
    Throwable item(*dynamic_cast<const Throwable*>(ItemHandler::getTemplate(id)));
    item.throwItem(pos, momentum, thrower);
    throwables.push_back(item);
}

void ProjectileHandler::addSpell(int tomeID, sf::Vector2f pos, sf::Vector2f destination, float channelTime)
{
    const Tome* tome = dynamic_cast<const Tome*>(ItemHandler::getTemplate(tomeID));

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

void ProjectileHandler::drawDebug(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (drawHitboxes)
        for (const Throwable& throwable : this->throwables)
            target.draw(throwable.getCollider(), states);
}

void ProjectileHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const Throwable& throwable : this->throwables)
        target.draw(throwable, states);
}
