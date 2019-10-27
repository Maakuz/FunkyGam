#pragma once
#include "SFML/Graphics.hpp"
#include "Entities/Item.h"


class UIHandler : public sf::Drawable
{
public:
    struct Inventory
    {
        static const int QUICKSLOT_COUNT = 5;
        static const int ITEM_SLOT_COUNT = 25;
        int quickslotItems[QUICKSLOT_COUNT];
        Item* itemSlots[ITEM_SLOT_COUNT];

        int selectedItemBarItem;
    };

    UIHandler();
    ~UIHandler() {};

    void initialize();

    const sf::View& getView() const { return this->view; };
    void setSelectedItem(int item);
    int getSelectedItem() const;


private:
    sf::VertexArray quickslotVertexArray;
    sf::Vector2u quickslotSize;
    sf::Vector2u quickslotPos;
    sf::Sprite quickslotSprites[Inventory::QUICKSLOT_COUNT];

    sf::View view;

    Inventory inventory;

    bool inventoryOpen;

    void setItemSlot(int whatSlot, int itemID);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};