#include "InventoryHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Handlers/TextureHandler.h"
#include "Handlers/ItemHandler.h";
#include "Misc/Definitions.h"
#include "Misc/MouseState.h"

InventoryHandler::InventoryHandler()
{
    this->inventory.selectedItemBarItem = 0;
    this->clickedItem = -1;

    this->inventoryOpen = false;
    this->currentToolTip = -1;
    this->quickslotsHidden = true;
    this->slotTexture = nullptr;
    this->toolTip.create(TextureHandler::get().getTexture(7), TextureHandler::get().getFont());
    this->toolTip.setCharacterSize(DEFAULT_TEXT_SIZE);

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
                addItem(std::stoi(args[0]), std::stoi(args[1]));

            else
                addItem(std::stoi(args[0]), ItemHandler::getTemplate(std::stoi(args[0]))->getStackLimit());

            updateQuickslotSprites();
            return "You got " + ItemHandler::getTemplate(std::stoi(args[0]))->getName() + "!";
        });
}

InventoryHandler::~InventoryHandler()
{
    for (Item* item : inventory.itemSlots)
        delete item;
}

void InventoryHandler::initialize(const sf::Texture* texture)
{
    this->slotTexture = texture;
    this->slotSize = texture->getSize();
    this->slotSize.x /= 2;
    this->quickslotPos.x = WIN_WIDTH / 2 - slotSize.x * 2.5f;
    sf::Vector2f textPos(slotSize.x - 16, slotSize.y - 16);

    for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
    {
        quickslots[i].setPosition(this->quickslotPos.x + i * slotSize.x, 0);
        quickslots[i].setSize(sf::Vector2f(this->slotSize));
        quickslots[i].setTexture(slotTexture);
        quickslots[i].setTextureRect(sf::IntRect(0, 0, slotSize.x, slotSize.y));

        quickslotSprites[i].setPosition(this->quickslotPos.x + (i * slotSize.x), 0);
        this->quickslotStackText[i].setFont(*TextureHandler::get().getFont());
        this->quickslotStackText[i].setCharacterSize(12);
        this->quickslotStackText[i].setPosition(quickslotSprites[i].getPosition() + textPos);
    }

    this->quickslots[0].setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

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

    addStartItems();
}

void InventoryHandler::update(float dt, sf::Vector2f mousePos)
{
    if (inventoryOpen)
    {
        int toolTipItem = -1;
        for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
        {
            if (this->inventorySlots[i].getGlobalBounds().contains(mousePos))
            {
                this->inventorySlots[i].setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

                if (inventory.itemSlots[i] != nullptr)
                {
                    toolTipItem = i;
                }

                if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && inventory.itemSlots[i] != nullptr && this->clickedItem == -1)
                {
                    this->clickedItem = i;
                }

                else if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && this->clickedItem != -1)
                {
                    this->swapItems(i, this->clickedItem);
                    this->clickedItem = -1;
                }
            }

            else if (i == this->clickedItem)
                this->inventorySlots[i].setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

            else
                this->inventorySlots[i].setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));
        }

        if (toolTipItem != -1)
        {

            this->currentToolTip = toolTipItem;
            this->toolTip.setText(inventory.itemSlots[toolTipItem]->getName());
            this->toolTip.resizeToFit();
            
            this->toolTip.setPos(mousePos);
        }

        else
            this->currentToolTip = -1;
    }
}

void InventoryHandler::addStartItems()
{
    std::ifstream file(DATA_PATH "StartItems.mop");

    if (!file.is_open())
    {
        printf("Cannot read StartItems.mop");
        system("pause");
        exit(-33);
    }

    int id;
    int amount;

    while (!file.eof())
    {
        file >> id >> amount;
        addItem(id, amount);
    }
}

int InventoryHandler::countItem(int itemID)
{
    int tally = 0;
    for (int i = 0; i < Inventory::ITEM_SLOT_COUNT; i++)
    {
        if (inventory.itemSlots[i] != nullptr && inventory.itemSlots[i]->getID() == itemID)
            tally += inventory.stackSizes[i];
    }

    return tally;
}

void InventoryHandler::setSelectedItem(int item)
{
    if (this->inventory.selectedItemBarItem == item)
        return;

    int prevSelected = this->inventory.selectedItemBarItem;
    this->inventory.selectedItemBarItem = item;

    for (int i = 0; i < 4; i++)
    {
        quickslots[item].setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));
        quickslots[prevSelected].setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));
    }
}

int InventoryHandler::getSelectedItemID() const
{
    if (inventory.itemSlots[inventory.selectedItemBarItem])
        return inventory.itemSlots[inventory.selectedItemBarItem]->getID();

    else
        return -1;
}

int InventoryHandler::useSelectedItem()
{
    int id = getSelectedItemID();
    if (id != -1)
    {
        if (!inventory.itemSlots[inventory.selectedItemBarItem]->isUseable())
            id = -1;

        else
        {
            inventory.stackSizes[inventory.selectedItemBarItem]--;
            stackText[inventory.selectedItemBarItem].setString(std::to_string(inventory.stackSizes[inventory.selectedItemBarItem]));

            if (inventory.stackSizes[inventory.selectedItemBarItem] <= 0)
            {
                delete inventory.itemSlots[inventory.selectedItemBarItem];
                inventory.itemSlots[inventory.selectedItemBarItem] = nullptr;
            }

            updateQuickslotSprites();
        }
    }

    return id;
}

void InventoryHandler::addItem(int itemID, int amount)
{
    bool updateQuickslots = false;
    while (amount > 0)
    {
        int i = 0;
        while (i < Inventory::ITEM_SLOT_COUNT &&
            inventory.itemSlots[i] != nullptr &&
            !(inventory.itemSlots[i]->getID() == itemID && inventory.itemSlots[i]->getStackLimit() > inventory.stackSizes[i]))
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
                inventory.itemSlots[i] = new Item(*ItemHandler::getTemplate(itemID));
                inventory.itemSlots[i]->setPosition(inventorySlots[i].getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (inventory.itemSlots[i]->getSize() / 2.f));
                int stack = std::min(amount, inventory.itemSlots[i]->getStackLimit() - inventory.stackSizes[i]);
                inventory.stackSizes[i] += stack;
                amount -= stack;
                stackText[i].setString(std::to_string(inventory.stackSizes[i]));
            }

            if (i < Inventory::QUICKSLOT_COUNT)
                updateQuickslots = true;
        }

        else if (i >= Inventory::ITEM_SLOT_COUNT)
            amount = 0;
    }
    if (updateQuickslots)
        updateQuickslotSprites();

}

int InventoryHandler::removeItem(int itemID, int amount)
{
    int tally = 0;
    for (int i = 0; i < Inventory::ITEM_SLOT_COUNT && amount > 0; i++)
    {
        if (inventory.itemSlots[i] != nullptr && inventory.itemSlots[i]->getID() == itemID)
        {
            int itemsTaken = std::min(amount, inventory.stackSizes[i]);
            tally += itemsTaken;

            inventory.stackSizes[i] -= itemsTaken;
            amount -= itemsTaken;
            stackText[i].setString(std::to_string(inventory.stackSizes[i]));

            if (inventory.stackSizes[i] <= 0)
            {
                delete inventory.itemSlots[i];
                inventory.itemSlots[i] = nullptr;
            }
        }
    }

    return tally;
}

void InventoryHandler::swapItems(int a, int b)
{
    if (inventory.itemSlots[a])
        inventory.itemSlots[a]->setPosition(inventorySlots[b].getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (inventory.itemSlots[a]->getSize() / 2.f));

    if (inventory.itemSlots[b])
        inventory.itemSlots[b]->setPosition(inventorySlots[a].getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (inventory.itemSlots[b]->getSize() / 2.f));

    std::swap(inventory.itemSlots[a], inventory.itemSlots[b]);
    std::swap(inventory.stackSizes[a], inventory.stackSizes[b]);

    stackText[a].setString(std::to_string(inventory.stackSizes[a]));
    stackText[b].setString(std::to_string(inventory.stackSizes[b]));

    if (a < Inventory::QUICKSLOT_COUNT || b < Inventory::QUICKSLOT_COUNT)
        this->updateQuickslotSprites();

}

void InventoryHandler::updateQuickslotSprites()
{
    for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
    {
        if (inventory.itemSlots[i])
        {
            quickslotSprites[i].setTexture(*inventory.itemSlots[i]->getTexture(), true);
            quickslotSprites[i].setPosition(quickslots[i].getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (inventory.itemSlots[i]->getSize() / 2.f));

            quickslotStackText[i].setString(stackText[i].getString());
        }
    }
}

void InventoryHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!quickslotsHidden)
    {
        for (int i = 0; i < Inventory::QUICKSLOT_COUNT; i++)
        {
            target.draw(quickslots[i], states);
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

        if (this->currentToolTip != -1)
            target.draw(toolTip, states);
    }
}
