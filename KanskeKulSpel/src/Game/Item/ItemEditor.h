#pragma once
#include <vector>
#include "Item.h"
#include "Consumable.h"
#include "Game/Item/Projectile/Spell/Tome.h"
#include "Projectile/Throwables/Throwable.h"
#include "Projectile/Spell/Fireball.h"
#include "Projectile/LightProjectile.h"

class ItemEditor : public sf::Drawable
{
public:
    ItemEditor();
    virtual ~ItemEditor();
    ItemEditor(ItemEditor&) = delete;

    void openWindow();
    void closeWindow() { m_open = false; };
    bool isOpen() const { return m_open; };
    void update(float dt, sf::Vector2f mouseWorldPos);
    void updateItems(float dt, sf::Vector2f mouseWorldPos);
    void updateSpells(float dt, sf::Vector2f mouseWorldPos);
    void updateProjectiles(float dt, sf::Vector2f mouseWorldPos);
private:
    enum class Tab 
    {
        items,
        spells,
        projectiles
    };

    Tab m_currentTab;

    bool m_open;
    std::vector<Entity*> m_items;
    std::vector<Spell*> m_spells;
    std::vector<LightProjectile> m_projectiles;
    std::vector<std::string> m_projectileNames;
    int m_currentItem;
    int m_currentSpell;
    int m_currentProjectile;
    sf::Vector2f m_pos;

    void showItemData(Entity* item);
    void showExplosionData(Explosion* explosion);
    void showThrowableData(Throwable* item);
    void showTomeData(Tome* item);
    void showConsumableData(Consumable* consumable);
    void showFireballData(Fireball* fireball);
    void showProjectileData(LightProjectile* projectile, std::string* name);

    void readItems();
    void readSpells();
    void readProjectiles();

    void writeItems();
    void writeSpells();
    void writeProjectiles();

    void clearItems();
    void clearSpells();
    void clearProjectiles();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

