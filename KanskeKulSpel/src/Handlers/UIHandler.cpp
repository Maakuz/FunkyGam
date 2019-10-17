#include "UIHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/Definitions.h"
#include "Handlers/ItemHandler.h";
UIHandler::UIHandler()
{
    this->selectedItem = 0;
}

void UIHandler::initialize()
{
    this->quickslotVertexArray.setPrimitiveType(sf::Quads);
    this->quickslotSize = TextureHandler::get().getTexture(5)->getSize();
    this->quickslotSize.x /= 2;
    this->quickslotPos.x = WIN_WIDTH / 2 - quickslotSize.x * 2.5;

    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        sf::Vertex v[4];
        v[0].position = sf::Vector2f(this->quickslotPos.x + (i * quickslotSize.x), 0);
        v[1].position = sf::Vector2f(this->quickslotPos.x + ((i + 1) * quickslotSize.x), 0);
        v[2].position = sf::Vector2f(this->quickslotPos.x + ((i + 1) * quickslotSize.x), quickslotSize.y);
        v[3].position = sf::Vector2f(this->quickslotPos.x + (i * quickslotSize.x), quickslotSize.y);

        v[0].texCoords = sf::Vector2f(0, 0);
        v[1].texCoords = sf::Vector2f(this->quickslotSize.x, 0);
        v[2].texCoords = sf::Vector2f(this->quickslotSize.x, this->quickslotSize.y);
        v[3].texCoords = sf::Vector2f(0, this->quickslotSize.y);

        for (int j = 0; j < 4; j++)
        {
            quickslotVertexArray.append(v[j]);
        }

        quickSlotItems[i] = i;
        quickSlotItems[4] = 3;//temp
        quickSlotSprites[i].setPosition(this->quickslotPos.x + (i * quickslotSize.x), 0);
        quickSlotSprites[i].setTexture(*ItemHandler::getTemplate(quickSlotItems[i]).getTexture(), true);
    }

    for (int i = 0; i < 4; i++)
    {
        quickslotVertexArray[i].texCoords.x += quickslotSize.x;
    }

    view = sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT));
}

void UIHandler::setSelectedItem(int item)
{
    int prevSelected = this->selectedItem * 4;
    this->selectedItem = item;
    item = item * 4;

    for (int i = 0; i < 4; i++)
    {
        quickslotVertexArray[item + i].texCoords.x += quickslotSize.x;
        quickslotVertexArray[prevSelected + i].texCoords.x -= quickslotSize.x;
    }
}

void UIHandler::setItemSlot(int whatSlot, int itemID)
{
    quickSlotItems[whatSlot] = itemID;
    if (itemID != -1)
        quickSlotSprites[whatSlot].setTexture(*ItemHandler::getTemplate(itemID).getTexture(), true);
}

void UIHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = TextureHandler::get().getTexture(5);

    target.draw(quickslotVertexArray, states);
    states.texture = nullptr;
    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        if (quickSlotItems[i] != -1)
            target.draw(quickSlotSprites[i], states);
    }
}
