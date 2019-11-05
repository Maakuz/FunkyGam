#pragma once
#include <vector>
#include "Item.h"
#include "Throwables/Throwable.h"

class ItemEditor
{
public:
    ItemEditor();
    ~ItemEditor();

    void openWindow();
    void closeWindow() { open = false; };
    bool isOpen() const { return open; };
    void update();
private:
    bool open;
    std::vector<Item*> items;
    int currentItem;
    sf::Vector2f pos;

    void showItemData(Item* item);
    void showThrowableData(Throwable* item);
    void readItems();
    void writeItems();
    void clearItems();
};

