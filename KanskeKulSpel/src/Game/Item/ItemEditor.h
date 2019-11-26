#pragma once
#include <vector>
#include "Item.h"
#include "Game/Item/Projectile/Spell/Tome.h"
#include "Projectile/Throwables/Throwable.h"
#include "Projectile/Spell/Fireball.h"
#include "Projectile/LightProjectile.h"

class ItemEditor
{
public:
    ItemEditor();
    ~ItemEditor();

    void openWindow();
    void closeWindow() { open = false; };
    bool isOpen() const { return open; };
    void update();
    void updateItems();
    void updateSpells();
    void updateProjectiles();
private:
    bool open;
    std::vector<Item*> items;
    std::vector<Spell*> spells;
    std::vector<LightProjectile> projectiles;
    int currentItem;
    int currentSpell;
    int currentProjectile;
    sf::Vector2f pos;

    void showItemData(Item* item);
    void showExplosionData(Explosion* explosion);
    void showThrowableData(Throwable* item);
    void showTomeData(Tome* item);
    void showFireballData(Fireball* fireball);
    void showProjectileData(LightProjectile* projectile);

    void readItems();
    void readSpells();
    void readProjectiles();

    void writeItems();
    void writeSpells();
    void writeProjectiles();

    void clearItems();
    void clearSpells();
    void clearProjectiles();
};

