#pragma once
#include "SFML/Graphics.hpp"
#include "Game/Entities/Items/Item.h"
#include "Game/Interface/TextBubble.h"
#include <vector>

class InventoryHandler : public sf::Drawable
{
public:
    InventoryHandler();
    ~InventoryHandler();

    void initialize(const sf::Texture* texture);

    void update(float dt, sf::Vector2f mousePos);

    void addStartItems();

    int countItem(int itemID);
    void setSelectedItem(int item);
    int getSelectedItemID() const;
    int useSelectedItem();
    void openInventory() { this->inventoryOpen = true; };
    void closeInventory() { this->inventoryOpen = false; };
    bool isInventoryOpen() const { return this->inventoryOpen; };
    void setQuickslotHidden(bool value) { this->quickslotsHidden = value; };

    void sortItems();
    void addItem(int itemID, int amount = 1);
    //Returns amount of items removed
    int removeItem(int itemID, int amount = 1);
private:

    static const int QUICKSLOT_COUNT = 5;
    static const int ITEM_SLOT_COUNT = 25;


    struct InventorySlot
    {
        Item* item;
        int size;
        sf::Text text;
        sf::RectangleShape rect;
    };

    InventorySlot slots[ITEM_SLOT_COUNT];
    
    struct Quickslot
    {
        sf::RectangleShape rect;
        sf::Text text;
        sf::Sprite sprite;
    };

    Quickslot quickslots[QUICKSLOT_COUNT];
    sf::Vector2u quickslotPos;
    int selectedQuickslotItem;

    sf::Vector2u slotSize;
    const sf::Texture* slotTexture;

    int currentToolTip;
    TextBubble toolTip;

    bool inventoryOpen;
    bool quickslotsHidden;
    int clickedItem;

    //return true if merged successfully without leftovers
    bool mergeStacks(int to, int from);

    void swapItems(int a, int b);
    void updateQuickslotSprites();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};