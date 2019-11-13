#pragma once
#include <vector>
#include "Item.h"
#include "Game/Item/Spell/Tome.h"
#include "Throwables/Throwable.h"
#include "Spell/Fireball.h"

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
private:
    bool open;
    std::vector<Item*> items;
    std::vector<Spell*> spells;
    int currentItem;
    int currentSpell;
    sf::Vector2f pos;

    void showItemData(Item* item);
    void showThrowableData(Throwable* item);
    void showTomeData(Tome* item);
    void showFireballData(Fireball* fireball);

    void readItems();
    void readSpells();

    void writeItems();
    void writeSpells();

    void clearItems();
    void clearSpells();
};

