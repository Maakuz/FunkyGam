#pragma once
#include "SFML/Graphics.hpp"


class UIHandler : public sf::Drawable
{
public:
    static const int QUICKSLOT_COUNT = 5;
    UIHandler();
    ~UIHandler() {};

    void initialize();

    const sf::View& getView() const { return this->view; };
    void setSelectedItem(int item);

    void setItemSlot(int whatSlot, int itemID);

private:
    sf::VertexArray quickslotVertexArray;
    sf::Vector2u quickslotSize;
    sf::Vector2u quickslotPos;
    int selectedItem;
    int quickSlotItems[QUICKSLOT_COUNT];
    sf::Sprite quickSlotSprites[QUICKSLOT_COUNT];

    sf::View view;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};