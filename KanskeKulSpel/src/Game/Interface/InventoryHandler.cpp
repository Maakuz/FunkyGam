#include "InventoryHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Misc/Definitions.h"
#include "Game/Misc/MouseState.h"
#include <fstream>

InventoryHandler::InventoryHandler()
{
    this->selectedQuickslotItem = 0;
    this->clickedItem = -1;

    this->inventoryOpen = false;
    this->currentToolTip = -1;
    this->quickslotsHidden = true;
    this->slotTexture = nullptr;
    this->toolTip.create(TextureHandler::get().getTexture(7), TextureHandler::get().getFont());
    this->toolTip.setCharacterSize(DEFAULT_TEXT_SIZE);

    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        this->slots[i].size = 0;
        this->slots[i].text.setFont(*TextureHandler::get().getFont());
        this->slots[i].text.setCharacterSize(12);
        this->slots[i].infinite = false;
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

    ConsoleWindow::get().addCommand("sortInventory", [&](Arguments args)->std::string
        {
            sortItems();
            return "Sorted";
        });
}

InventoryHandler::~InventoryHandler()
{
    for (InventorySlot& item : slots)
        delete item.item;
}

void InventoryHandler::initialize(const sf::Texture* texture)
{
    for (InventorySlot& item : slots)
        delete item.item;

    this->slotTexture = texture;
    this->slotSize = texture->getSize();
    this->slotSize.x /= 2;
    this->quickslotPos.x = WIN_WIDTH / 2 - slotSize.x * 2.5f;
    sf::Vector2f textPos(slotSize.x - 16, slotSize.y - 16);

    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        this->quickslots[i].rect.setPosition(this->quickslotPos.x + i * this->slotSize.x, 0);
        this->quickslots[i].rect.setSize(sf::Vector2f(this->slotSize));
        this->quickslots[i].rect.setTexture(this->slotTexture);
        this->quickslots[i].rect.setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));

        this->quickslots[i].sprite.setPosition(this->quickslotPos.x + (i * slotSize.x), 0);
        
        this->quickslots[i].text.setFont(*TextureHandler::get().getFont());
        this->quickslots[i].text.setCharacterSize(12);
        this->quickslots[i].text.setPosition(quickslots[i].sprite.getPosition() + textPos);
    }

    this->quickslots[0].rect.setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

    updateQuickslotSprites();

    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        this->slots[i].item = nullptr;
        int x = (WIN_WIDTH / 2.f) - (this->slotSize.x * 5 / 2) + ((i % 5) * slotSize.x);
        int y = (WIN_HEIGHT / 2.f) - (this->slotSize.y * 5 / 2) + ((i / 5) * slotSize.y);
        this->slots[i].rect.setPosition(x, y);
        this->slots[i].rect.setSize(sf::Vector2f(this->slotSize));
        this->slots[i].rect.setTexture(slotTexture);
        this->slots[i].rect.setTextureRect(sf::IntRect(0, 0, slotSize.x, slotSize.y));

        this->slots[i].text.setPosition(x + textPos.x, y + textPos.y);
    }

    addStartItems();
}

void InventoryHandler::update(float dt, sf::Vector2f mousePos)
{
    if (inventoryOpen)
    {
        int toolTipItem = -1;
        for (int i = 0; i < ITEM_SLOT_COUNT; i++)
        {
            if (this->slots[i].rect.getGlobalBounds().contains(mousePos))
            {
                this->slots[i].rect.setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

                if (this->slots[i].item != nullptr)
                {
                    toolTipItem = i;
                }

                if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && this->slots[i].item != nullptr && this->clickedItem == -1)
                {
                    this->clickedItem = i;
                }

                else if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && this->clickedItem != -1)
                {
                    if (!this->mergeStacks(i, this->clickedItem))
                        this->swapItems(i, this->clickedItem);
                    this->clickedItem = -1;
                }
            }

            else if (i == this->clickedItem)
                this->slots[i].rect.setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));

            else
                this->slots[i].rect.setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));
        }

        if (toolTipItem != -1)
        {

            this->currentToolTip = toolTipItem;
            this->toolTip.setText(this->slots[toolTipItem].item->getName());
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
    for (InventorySlot& slot : this->slots)
        if (slot.item != nullptr && slot.item->getID() == itemID)
            tally += slot.size;

    return tally;
}

void InventoryHandler::setSelectedItem(int item)
{
    if (this->selectedQuickslotItem == item)
        return;

    int prevSelected = this->selectedQuickslotItem;
    this->selectedQuickslotItem = item;

    for (int i = 0; i < 4; i++)
    {
        quickslots[item].rect.setTextureRect(sf::IntRect(this->slotSize.x, 0, this->slotSize.x, this->slotSize.y));
        quickslots[prevSelected].rect.setTextureRect(sf::IntRect(0, 0, this->slotSize.x, this->slotSize.y));
    }
}

int InventoryHandler::getSelectedItemID() const
{
    if (slots[selectedQuickslotItem].item)
        return slots[selectedQuickslotItem].item->getID();

    else
        return -1;
}

int InventoryHandler::useSelectedItem()
{
    int id = getSelectedItemID();
    if (id != -1)
    {
        if (!this->slots[this->selectedQuickslotItem].item->isUseable())
            id = -1;

        else if (!this->slots[this->selectedQuickslotItem].infinite)
        {
            this->slots[this->selectedQuickslotItem].size--;
            this->slots[this->selectedQuickslotItem].text.setString(std::to_string(this->slots[this->selectedQuickslotItem].size));

            if (this->slots[this->selectedQuickslotItem].size <= 0)
            {
                delete this->slots[this->selectedQuickslotItem].item;
                this->slots[this->selectedQuickslotItem].item = nullptr;
            }

            updateQuickslotSprites();
        }
    }


    return id;
}

void InventoryHandler::sortItems()
{
    //Sort out empty spaces
    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        for (int j = i; j < ITEM_SLOT_COUNT; j++)
        {
            if (slots[i].item == nullptr && slots[j].item != nullptr)
                swapItems(i, j);
        }
    }

    //Merge what can be merged
    for (int i = 0; i < ITEM_SLOT_COUNT-1; i++)
    {
        for (int j = i + 1; j < ITEM_SLOT_COUNT; j++)
        {
            if (slots[i].item && slots[j].item && slots[i].item->getID() == slots[j].item->getID())
            {
                bool fullyMerged = mergeStacks(i, j);
                if (!fullyMerged)
                {
                    i++;
                    swapItems(i, j);
                }
            }
        }
    }

    //And sort empty spaces once more
    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        for (int j = i; j < ITEM_SLOT_COUNT; j++)
        {
            if (slots[i].item == nullptr && slots[j].item != nullptr)
                swapItems(i, j);
        }
    }
}

void InventoryHandler::addItem(int itemID, int amount)
{
    bool updateQuickslots = false;
    if (amount == -1)
    {
        bool first = true;
        for (int i = 0; i < ITEM_SLOT_COUNT; i++)
        {
            if (slots[i].item == nullptr && first)
            {
                this->slots[i].item = new Item(*ItemHandler::getTemplate(itemID));
                this->slots[i].item->setPosition(this->slots[i].rect.getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (this->slots[i].item->getTextureSize() / 2.f));
                slots[i].infinite = true;
                slots[i].size = 8;
                slots[i].text.setString("Inf");
                first = false;            
            }

            else if (slots[i].item && slots[i].item->getID() == itemID && first)
            {
                slots[i].infinite = true;
                slots[i].size = 8;
                slots[i].text.setString("Inf");
                first = false;
            }

            else if (slots[i].item && slots[i].item->getID() == itemID)
            {
                removeAt(i);
            }
        }
        updateQuickslots = true;
    }

    else
    {
        while (amount > 0)
        {
            int i = 0;
            while (i < ITEM_SLOT_COUNT &&
                this->slots[i].item != nullptr &&
                !(this->slots[i].item->getID() == itemID && this->slots[i].item->getStackLimit() > this->slots[i].size))
            {
                i++;
            }

            if (i < ITEM_SLOT_COUNT)
            {
                if (this->slots[i].item)
                {
                    int stack = std::min(amount, this->slots[i].item->getStackLimit() - this->slots[i].size);
                    this->slots[i].size += stack;
                    amount -= stack;
                    this->slots[i].text.setString(std::to_string(this->slots[i].size));
                }

                else
                {
                    this->slots[i].item = new Item(*ItemHandler::getTemplate(itemID));
                    this->slots[i].item->setPosition(this->slots[i].rect.getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (this->slots[i].item->getTextureSize() / 2.f));
                    int stack = std::min(amount, this->slots[i].item->getStackLimit() - this->slots[i].size);
                    this->slots[i].size += stack;
                    amount -= stack;
                    this->slots[i].text.setString(std::to_string(this->slots[i].size));
                }

                if (i < QUICKSLOT_COUNT)
                    updateQuickslots = true;
            }

            else if (i >= ITEM_SLOT_COUNT)
                amount = 0;
        }
    }
    if (updateQuickslots)
        updateQuickslotSprites();
}

int InventoryHandler::removeItem(int itemID, int amount)
{
    int tally = 0;
    for (int i = 0; i < ITEM_SLOT_COUNT && amount > 0; i++)
    {
        if (slots[i].item != nullptr && slots[i].item->getID() == itemID)
        {
            int itemsTaken = std::min(amount, slots[i].size);
            tally += itemsTaken;

            slots[i].size -= itemsTaken;
            amount -= itemsTaken;
            slots[i].text.setString(std::to_string(slots[i].size));

            if (slots[i].size <= 0)
            {
                delete slots[i].item;
                slots[i].item = nullptr;
            }
        }
    }

    return tally;
}

void InventoryHandler::removeAt(int slot)
{
    delete slots[slot].item;
    slots[slot].item = nullptr;
    slots[slot].infinite = false;
    slots[slot].size = 0;
}

bool InventoryHandler::mergeStacks(int to, int from)
{
    if (!slots[to].item || !slots[from].item || slots[to].item->getID() != slots[from].item->getID())
        return false;

    int avaliableSpace = slots[to].item->getStackLimit() - slots[to].size;

    slots[to].size += std::min(slots[from].size, avaliableSpace);
    slots[from].size -= std::min(slots[from].size, avaliableSpace);

    slots[to].text.setString(std::to_string(slots[to].size));
    slots[from].text.setString(std::to_string(slots[from].size));

    if (slots[from].size <= 0)
    {
        delete slots[from].item;
        slots[from].item = nullptr;
        return true;
    }

    return false;
}

void InventoryHandler::swapItems(int a, int b)
{
    if (this->slots[a].item)
        this->slots[a].item->setPosition(this->slots[b].rect.getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (this->slots[a].item->getTextureSize() / 2.f));

    if (this->slots[b].item)
        this->slots[b].item->setPosition(this->slots[a].rect.getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (this->slots[b].item->getTextureSize() / 2.f));

    std::swap(this->slots[a].item, this->slots[b].item);
    std::swap(this->slots[a].size, this->slots[b].size);
    std::swap(this->slots[a].infinite, this->slots[b].infinite);

    this->slots[a].text.setString(std::to_string(this->slots[a].size));
    this->slots[b].text.setString(std::to_string(this->slots[b].size));

    if (a < QUICKSLOT_COUNT || b < QUICKSLOT_COUNT)
        this->updateQuickslotSprites();

}

void InventoryHandler::updateQuickslotSprites()
{
    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        if (this->slots[i].item)
        {
            quickslots[i].sprite.setTexture(*this->slots[i].item->getTexture(), true);
            quickslots[i].sprite.setPosition(quickslots[i].rect.getPosition() + (sf::Vector2f(this->slotSize) / 2.f) - (slots[i].item->getTextureSize() / 2.f));

            quickslots[i].text.setString(slots[i].text.getString());
        }
    }
}

void InventoryHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!quickslotsHidden)
    {
        for (int i = 0; i < QUICKSLOT_COUNT; i++)
        {
            target.draw(quickslots[i].rect, states);
            if (this->slots[i].item)
            {
                target.draw(quickslots[i].sprite, states);
                target.draw(quickslots[i].text, states);
            }
        }
    }

    if (inventoryOpen)
    {
        for (int i = 0; i < ITEM_SLOT_COUNT; i++)
        {
            target.draw(slots[i].rect, states);
            if (this->slots[i].item)
            {
                target.draw(*this->slots[i].item, states);
                target.draw(this->slots[i].text, states);
            }
        }

        if (this->currentToolTip != -1)
            target.draw(toolTip, states);
    }
}
