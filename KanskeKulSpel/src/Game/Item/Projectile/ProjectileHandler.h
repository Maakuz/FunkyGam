#pragma once
#include <unordered_map>
#include "Game/Entities/Player.h"
#include "Renderer/DebugDrawable.h"
#include "Game/Item/Projectile/Throwables/Throwable.h"
#include "Game/Item/Projectile/Spell/Fireball.h"
#include "Game/Item/Projectile/LightProjectile.h"

class ProjectileHandler : public sf::Drawable, public DebugDrawable
{
public:
    ProjectileHandler();
    virtual ~ProjectileHandler();

    void update(float dt, Player* player);
    void loadTemplates();
    void queueColliders();
    void reset();

    static void addThrowable(int id, sf::Vector2f pos, sf::Vector2f momentum, DamageComp::DamageOrigin origin);
    static void addSpell(std::string spellID, sf::Vector2f pos, sf::Vector2f destination, float channelTime);
    static void addProjectile(int projectileID, sf::Vector2f pos, sf::Vector2f direction, DamageComp::DamageOrigin origin);

    virtual void drawDebug(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    bool drawHitboxes;

    static std::unordered_map<std::string, const Spell*> spellTemplates;
    static std::vector<LightProjectile> projectileTemplates;

    static std::vector<Throwable> throwables;
    static std::vector<LightProjectile*> projectiles;
    static std::vector<Spell*> spells;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};