#pragma once
#include "SFML/Graphics.hpp"
#include "Entities/Item.h"
#include <vector>

class UIHandler : public sf::Drawable
{
public:
    UIHandler();
    ~UIHandler();

    void initialize();

    void update(float dt, sf::Vector2f mousePos);

    void addStartItems();

    const sf::View& getView() const { return this->view; };
    void setSelectedItem(int item);
    int getSelectedItemID() const;
    int useSelectedItem();
    void openInventory() { this->inventoryOpen = true; };
    void closeInventory() { this->inventoryOpen = false; };
    bool isInventoryOpen() const { return this->inventoryOpen; };
    void setQuickslotHidden(bool value) { this->quickslotsHidden = value; };

    void addInventoryItem(int itemID, int amount = 1);

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
    std::vector<Item*> stash;
    std::vector<sf::RectangleShape> stashSlots;

    sf::RectangleShape quickslots[Inventory::QUICKSLOT_COUNT];
    sf::Text quickslotStackText[Inventory::QUICKSLOT_COUNT];
    sf::Sprite quickslotSprites[Inventory::QUICKSLOT_COUNT];
    sf::Vector2u quickslotPos;
    sf::Vector2u slotSize;
    sf::RectangleShape inventorySlots[Inventory::ITEM_SLOT_COUNT];
    sf::Text stackText[Inventory::ITEM_SLOT_COUNT];
    const sf::Texture* slotTexture;

    sf::View view;


    bool inventoryOpen;
    bool quickslotsHidden;

    void updateQuickslotSprites();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};