#include "UIHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/Definitions.h"

UIHandler::UIHandler()
{
    this->quickslotCount = 0;
    this->selectedItem = 0;
}

void UIHandler::initialize()
{
    this->quickslotCount = 5;
    this->quickslotVertexArray.setPrimitiveType(sf::Quads);
    this->quickslotSize = TextureHandler::get().getTexture(TextureHandler::ui::quickSlot)->getSize();
    this->quickslotSize.x /= 2;
    this->quickslotPos.x = WIN_WIDTH / 2 - quickslotSize.x * 2.5;

    for (int i = 0; i < quickslotCount; i++)
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

void UIHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = TextureHandler::get().getTexture(TextureHandler::ui::quickSlot);

    target.draw(quickslotVertexArray, states);
}
