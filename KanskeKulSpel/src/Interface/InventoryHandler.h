#pragma once
#include "SFML/Graphics.hpp"
#include "Entities/Items/Item.h"
#include "Interface/TextBubble.h"
#include <vector>

class InventoryHandler : public sf::Drawable
{
public:
    InventoryHandler();
    ~InventoryHandler();

    void initialize(const sf::Texture* texture);

    void update(float dt, sf::Vector2f mousePos);

    void addStartItems();


    void setSelectedItem(int item);
    int getSelectedItemID() const;
    int useSelectedItem();
    void openInventory() { this->inventoryOpen = true; };
    void closeInventory() { this->inventoryOpen = false; };
    bool isInventoryOpen() const { return this->inventoryOpen; };
    void setQuickslotHidden(bool value) { this->quickslotsHidden = value; };

    void addItem(int itemID, int amount = 1);
private:
    struct Inventory
    {
        static const int QUICKSLOT_COUNT = 5;
        static const int ITEM_SLOT_COUNT = 25;
        Item* itemSlots[ITEM_SLOT_COUNT];
        int stackSizes[ITEM_SLOT_COUNT];

        int selectedItemBarItem;
    };
    Inventory inventory;
    
    sf::RectangleShape quickslots[Inventory::QUICKSLOT_COUNT];
    sf::Text quickslotStackText[Inventory::QUICKSLOT_COUNT];
    sf::Sprite quickslotSprites[Inventory::QUICKSLOT_COUNT];
    sf::Vector2u quickslotPos;
    sf::RectangleShape inventorySlots[Inventory::ITEM_SLOT_COUNT];
    sf::Text stackText[Inventory::ITEM_SLOT_COUNT];

    sf::Vector2u slotSize;
    const sf::Texture* slotTexture;

    int currentToolTip;
    TextBubble toolTip;

    bool inventoryOpen;
    bool quickslotsHidden;
    int clickedItem;

    void swapItems(int a, int b);
    void updateQuickslotSprites();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};