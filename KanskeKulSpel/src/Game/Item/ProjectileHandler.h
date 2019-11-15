#pragma once
#include <unordered_map>
#include "Game/Entities/Player.h"
#include "Renderer/DebugDrawable.h"
#include "Game/Item/Throwables/Throwable.h"
#include "Game/Item/Spell/Fireball.h"

class ProjectileHandler : public sf::Drawable, public DebugDrawable
{
public:
    ProjectileHandler();
    ~ProjectileHandler();

    void update(float dt, Player* player);
    void loadTemplates();
    void queueColliders();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, Entity* thrower);
    static void addSpell(int tomeID, sf::Vector2f pos, sf::Vector2f destination, float channelTime);

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    bool drawHitboxes;


    static std::unordered_map<std::string, const Spell*> spellTemplates;

    static std::vector<Throwable> throwables;
    static std::vector<Spell*> spells;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};