#include "UIHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/Definitions.h"
#include "Handlers/ItemHandler.h";
#include "Misc/ConsoleWindow.h"

UIHandler::UIHandler()
{
    this->inventory.selectedItemBarItem = 0;

    this->inventoryOpen = false;
    this->quickslotsHidden = true;
    this->slotTexture = nullptr;

    for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
    {
        this->inventory.stackSizes[i] = 0;
        this->stackText[i].setFont(*TextureHandler::get().getFont());
        this->stackText[i].setCharacterSize(12);
    }

    ConsoleWindow::get().addCommand("getItem", [&](Arguments args)->std::string 
        {
            if (args.empty())
                return "You get nothing! (missing arg int itemID)";
            

            if (args.size() >= 2)
                addInventoryItem(std::stoi(args[0]), std::stoi(args[1]));

            else
                addInventoryItem(std::stoi(args[0]));
            
            updateQuickslotSprites();
            return "You got " + ItemHandler::getTemplate(std::stoi(args[0])).getName() + "!";
        });
}

UIHandler::~UIHandler()
{
    for (Item* item : inventory.itemSlots)
        delete item;
}

void UIHandler::initialize()
{
    this->quickslotVertexArray.setPrimitiveType(sf::Quads);
    this->slotTexture = TextureHandler::get().getTexture(5);
    this->slotSize = slotTexture->getSize();
    this->slotSize.x /= 2;
    this->quickslotPos.x = WIN_WIDTH / 2 - slotSize.x * 2.5;
    sf::Vector2f textPos(slotSize.x - 16, slotSize.y - 16);

    for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
    {
        sf::Vertex v[4];
        v[0].position = sf::Vector2f(this->quickslotPos.x + (i * slotSize.x), 0);
        v[1].position = sf::Vector2f(this->quickslotPos.x + ((i + 1) * slotSize.x), 0);
        v[2].position = sf::Vector2f(this->quickslotPos.x + ((i + 1) * slotSize.x), slotSize.y);
        v[3].position = sf::Vector2f(this->quickslotPos.x + (i * slotSize.x), slotSize.y);

        v[0].texCoords = sf::Vector2f(0, 0);
        v[1].texCoords = sf::Vector2f(this->slotSize.x, 0);
        v[2].texCoords = sf::Vector2f(this->slotSize.x, this->slotSize.y);
        v[3].texCoords = sf::Vector2f(0, this->slotSize.y);

        for (int j = 0; j < 4; j++)
        {
            quickslotVertexArray.append(v[j]);
        }

        quickslotSprites[i].setPosition(this->quickslotPos.x + (i * slotSize.x), 0);
        this->quickslotStackText[i].setFont(*TextureHandler::get().getFont());
        this->quickslotStackText[i].setCharacterSize(12);
        this->quickslotStackText[i].setPosition(quickslotSprites[i].getPosition() + textPos);
    }

    for (int i = 0; i < 4; i++)
    {
        quickslotVertexArray[i].texCoords.x += slotSize.x;
    }

    updateQuickslotSprites();

    for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
    {
        inventory.itemSlots[i] = nullptr;
        int x = (WIN_WIDTH / 2.f) - (this->slotSize.x * 5 / 2) + ((i % 5) * slotSize.x);
        int y = (WIN_HEIGHT / 2.f) - (this->slotSize.y * 5 / 2) + ((i / 5) * slotSize.y);
        inventorySlots[i].setPosition(x, y);
        inventorySlots[i].setSize(sf::Vector2f(this->slotSize));
        inventorySlots[i].setTexture(slotTexture);
        inventorySlots[i].setTextureRect(sf::IntRect(0, 0, slotSize.x, slotSize.y));

        stackText[i].setPosition(x + textPos.x, y + textPos.y);

    }

    view = sf::View(sf::FloatRect(0, 0, WIN_WIDTH, WIN_HEIGHT));
}

void UIHandler::update(float dt, sf::Vector2f mousePos)
{
    if (inventoryOpen)
    {
        for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
        {
            if (this->inventorySlots[i].getGlobalBounds().contains(mousePos))
                this->inventorySlots[i].setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

            else
                this->inventorySlots[i].setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));
        }
    }
}

void UIHandler::setSelectedItem(int item)
{
    int prevSelected = this->inventory.selectedItemBarItem * 4;
    this->inventory.selectedItemBarItem = item;
    item = item * 4;

    for (int i = 0; i < 4; i++)
    {
        quickslotVertexArray[item + i].texCoords.x += slotSize.x;
        quickslotVertexArray[prevSelected + i].texCoords.x -= slotSize.x;
    }
}

int UIHandler::getSelectedItemID() const 
{ 
    if (inventory.itemSlots[inventory.selectedItemBarItem])
        return inventory.itemSlots[inventory.selectedItemBarItem]->getID();

    else
        return -1;
}

int UIHandler::useSelectedItem()
{
    int id = getSelectedItemID();
    if (id != -1)
    {
        inventory.stackSizes[inventory.selectedItemBarItem]--;
        stackText[inventory.selectedItemBarItem].setString(std::to_string(inventory.stackSizes[inventory.selectedItemBarItem]));
        
        if (inventory.stackSizes[inventory.selectedItemBarItem] <= 0)
        {
            delete inventory.itemSlots[inventory.selectedItemBarItem];
            inventory.itemSlots[inventory.selectedItemBarItem] = nullptr;
        }

    }

    return id;
}

void UIHandler::addInventoryItem(int itemID, int amount)
{
    while (amount > 0)
    {
        int i = 0;
        while(  inventory.itemSlots[i] != nullptr &&
                !(inventory.itemSlots[i]->getID() == itemID && inventory.itemSlots[i]->getStackLimit() > inventory.stackSizes[i]) &&
                i < Inventory::ITEM_SLOT_COUNT)
        {
            i++;
        }

        if (i < Inventory::ITEM_SLOT_COUNT)
        {
            if (inventory.itemSlots[i])
            {
                int stack = std::min(amount, inventory.itemSlots[i]->getStackLimit() - inventory.stackSizes[i]);
                inventory.stackSizes[i] += stack;
                amount -= stack;
                stackText[i].setString(std::to_string(inventory.stackSizes[i]));
            }

            else
            {
                inventory.itemSlots[i] = new Item(ItemHandler::getTemplate(itemID));
                inventory.itemSlots[i]->setPosition(inventorySlots[i].getPosition());
                int stack = std::min(amount, inventory.itemSlots[i]->getStackLimit() - inventory.stackSizes[i]);
                inventory.stackSizes[i] += stack;
                amount -= stack;
                stackText[i].setString(std::to_string(inventory.stackSizes[i]));
            }
        }

        else if (i > Inventory::ITEM_SLOT_COUNT)
            amount = 0;
    }
}

void UIHandler::updateQuickslotSprites()
{
    for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
    {
        if (inventory.itemSlots[i])
            quickslotSprites[i].setTexture(*inventory.itemSlots[i]->getTexture(), true);

        quickslotStackText[i].setString(stackText[i].getString());
    }
}

void UIHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!quickslotsHidden)
    {
        states.texture = TextureHandler::get().getTexture(5);

        target.draw(quickslotVertexArray, states);
        states.texture = nullptr;
        for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
        {
            if (inventory.itemSlots[i])
            {
                target.draw(quickslotSprites[i], states);
                target.draw(quickslotStackText[i], states);
            }
        }
    }

    if (inventoryOpen)
    {
        for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
        {
            target.draw(inventorySlots[i], states);
            if (inventory.itemSlots[i])
            {
                target.draw(*inventory.itemSlots[i], states);
                target.draw(stackText[i], states);
            }
        }
    }
}
