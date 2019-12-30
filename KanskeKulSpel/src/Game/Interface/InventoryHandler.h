#pragma once
#include "SFML/Graphics.hpp"
#include "Game/Item/Item.h"
#include "Game/Interface/TextBubble.h"
#include <vector>

class InventoryHandler : public sf::Drawable
{
public:
    InventoryHandler();
    virtual ~InventoryHandler();

    void initialize(const sf::Texture* texture);

    void update(float dt, sf::Vector2f mousePos);

    void addStartItems();

    int countItem(int itemID);
    void setSelectedItem(int item);
    int getSelectedItemID() const;
    int useSelectedItem();
    void openInventory() { this->m_inventoryOpen = true; };
    void closeInventory() { this->m_inventoryOpen = false; };
    bool isInventoryOpen() const { return this->m_inventoryOpen; };
    void setQuickslotHidden(bool value) { this->m_quickslotsHidden = value; };

    void sortItems();
    void addItem(int itemID, int amount = 1);
    //Returns amount of items removed
    int removeItem(int itemID, int amount = 1);
    void removeAt(int slot);
private:

    static const int QUICKSLOT_COUNT = 5;
    static const int ITEM_SLOT_COUNT = 25;


    struct InventorySlot
    {
        Entity* item;
        int size;
        sf::Text text;
        sf::RectangleShape rect;
        bool infinite;
    };

    InventorySlot m_slots[ITEM_SLOT_COUNT];
    
    struct Quickslot
    {
        sf::RectangleShape rect;
        sf::Text text;
        sf::Sprite sprite;
    };

    Quickslot m_quickslots[QUICKSLOT_COUNT];
    sf::Vector2u m_quickslotPos;
    int m_selectedQuickslotItem;

    sf::Vector2u m_slotSize;
    const sf::Texture* m_slotTexture;

    int m_currentToolTip;
    TextBubble m_toolTip;

    bool m_inventoryOpen;
    bool m_quickslotsHidden;
    int m_clickedItem;

    //return true if merged successfully without leftovers
    bool mergeStacks(int to, int from);

    void swapItems(int a, int b);
    void updateQuickslotSprites();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};