#include "InventoryHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Misc/Definitions.h"
#include "Game/Misc/MouseState.h"
#include <fstream>

InventoryHandler::InventoryHandler()
{
    m_selectedQuickslotItem = 0;
    m_clickedItem = -1;

    m_inventoryOpen = false;
    m_currentToolTip = -1;
    m_quickslotsHidden = true;
    m_slotTexture = nullptr;
    m_toolTip.create(TextureHandler::get().getTexture(7), TextureHandler::get().getFont());
    m_toolTip.setCharacterSize(DEFAULT_TEXT_SIZE);

    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        m_slots[i].size = 0;
        m_slots[i].text.setFont(*TextureHandler::get().getFont());
        m_slots[i].text.setCharacterSize(12);
        m_slots[i].infinite = false;
    }

    ConsoleWindow::get().addCommand("getItem", [&](Arguments args)->std::string
        {
            if (args.empty())
                return "You get nothing! (missing arg int itemID)";


            if (args.size() >= 2)
                addItem(std::stoi(args[0]), std::stoi(args[1]));

            else
                addItem(std::stoi(args[0]), ItemHandler::getTemplate(std::stoi(args[0]))->getLogisticsComp()->stackLimit);

            updateQuickslotSprites();
            return "You got " + ItemHandler::getTemplate(std::stoi(args[0]))->getLogisticsComp()->name + "!";
        });

    ConsoleWindow::get().addCommand("sortInventory", [&](Arguments args)->std::string
        {
            sortItems();
            return "Sorted";
        });
}

InventoryHandler::~InventoryHandler()
{
    for (InventorySlot& item : m_slots)
        delete item.item;
}

void InventoryHandler::initialize(const sf::Texture* texture)
{
    for (InventorySlot& item : m_slots)
        delete item.item;

    m_slotTexture = texture;
    m_slotSize = texture->getSize();
    m_slotSize.x /= 2;
    m_quickslotPos.x = WIN_WIDTH / 2 - m_slotSize.x * 2.5f;
    sf::Vector2f textPos(m_slotSize.x - 16, m_slotSize.y - 16);

    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        m_quickslots[i].rect.setPosition(m_quickslotPos.x + i * m_slotSize.x, 0);
        m_quickslots[i].rect.setSize(sf::Vector2f(m_slotSize));
        m_quickslots[i].rect.setTexture(m_slotTexture);
        m_quickslots[i].rect.setTextureRect(sf::IntRect(0, 0, m_slotSize.x, m_slotSize.y));

        m_quickslots[i].sprite.setPosition(m_quickslotPos.x + (i * m_slotSize.x), 0);
        
        m_quickslots[i].text.setFont(*TextureHandler::get().getFont());
        m_quickslots[i].text.setCharacterSize(12);
        m_quickslots[i].text.setPosition(m_quickslots[i].sprite.getPosition() + textPos);
    }

    m_quickslots[0].rect.setTextureRect(sf::IntRect(m_slotSize.x, 0, m_slotSize.x, m_slotSize.y));

    updateQuickslotSprites();

    for (int i = 0; i < ITEM_SLOT_COUNT; i++)
    {
        m_slots[i].item = nullptr;
        int x = (WIN_WIDTH / 2.f) - (m_slotSize.x * 5 / 2) + ((i % 5) * m_slotSize.x);
        int y = (WIN_HEIGHT / 2.f) - (m_slotSize.y * 5 / 2) + ((i / 5) * m_slotSize.y);
        m_slots[i].rect.setPosition(x, y);
        m_slots[i].rect.setSize(sf::Vector2f(m_slotSize));
        m_slots[i].rect.setTexture(m_slotTexture);
        m_slots[i].rect.setTextureRect(sf::IntRect(0, 0, m_slotSize.x, m_slotSize.y));

        m_slots[i].text.setPosition(x + textPos.x, y + textPos.y);
    }

    addStartItems();
}

void InventoryHandler::update(float dt, sf::Vector2f mousePos)
{
    if (m_inventoryOpen)
    {
        int toolTipItem = -1;
        for (int i = 0; i < ITEM_SLOT_COUNT; i++)
        {
            if (m_slots[i].rect.getGlobalBounds().contains(mousePos))
            {
                m_slots[i].rect.setTextureRect(sf::IntRect(m_slotSize.x, 0, m_slotSize.x, m_slotSize.y));

                if (m_slots[i].item != nullptr)
                {
                    toolTipItem = i;
                }

                if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && m_slots[i].item != nullptr && m_clickedItem == -1)
                {
                    m_clickedItem = i;
                }

                else if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left) && m_clickedItem != -1)
                {
                    if (!mergeStacks(i, m_clickedItem))
                        swapItems(i, m_clickedItem);
                    m_clickedItem = -1;
                }
            }

            else if (i == m_clickedItem)
                m_slots[i].rect.setTextureRect(sf::IntRect(m_slotSize.x, 0, m_slotSize.x, m_slotSize.y));

            else
                m_slots[i].rect.setTextureRect(sf::IntRect(0, 0, m_slotSize.x, m_slotSize.y));
        }

        if (toolTipItem != -1)
        {

            m_currentToolTip = toolTipItem;
            m_toolTip.setText(m_slots[toolTipItem].item->getLogisticsComp()->name);
            m_toolTip.resizeToFit();
            
            m_toolTip.setPos(mousePos);
        }

        else
            m_currentToolTip = -1;
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
    for (InventorySlot& slot : m_slots)
        if (slot.item != nullptr && slot.item->getLogisticsComp()->id == itemID)
            tally += slot.size;

    return tally;
}

void InventoryHandler::setSelectedItem(int item)
{
    if (m_selectedQuickslotItem == item)
        return;

    int prevSelected = m_selectedQuickslotItem;
    m_selectedQuickslotItem = item;

    for (int i = 0; i < 4; i++)
    {
        m_quickslots[item].rect.setTextureRect(sf::IntRect(m_slotSize.x, 0, m_slotSize.x, m_slotSize.y));
        m_quickslots[prevSelected].rect.setTextureRect(sf::IntRect(0, 0, m_slotSize.x, m_slotSize.y));
    }
}

int InventoryHandler::getSelectedItemID() const
{
    if (m_slots[m_selectedQuickslotItem].item)
        return m_slots[m_selectedQuickslotItem].item->getLogisticsComp()->id;

    else
        return -1;
}

int InventoryHandler::useSelectedItem()
{
    int id = getSelectedItemID();
    if (id != -1)
    {
        if (!m_slots[m_selectedQuickslotItem].item->getLogisticsComp()->useable)
            id = -1;

        else if (!m_slots[m_selectedQuickslotItem].infinite)
        {
            m_slots[m_selectedQuickslotItem].size--;
            m_slots[m_selectedQuickslotItem].text.setString(std::to_string(m_slots[m_selectedQuickslotItem].size));

            if (m_slots[m_selectedQuickslotItem].size <= 0)
            {
                delete m_slots[m_selectedQuickslotItem].item;
                m_slots[m_selectedQuickslotItem].item = nullptr;
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
            if (m_slots[i].item == nullptr && m_slots[j].item != nullptr)
                swapItems(i, j);
        }
    }

    //Merge what can be merged
    for (int i = 0; i < ITEM_SLOT_COUNT-1; i++)
    {
        for (int j = i + 1; j < ITEM_SLOT_COUNT; j++)
        {
            if (m_slots[i].item && m_slots[j].item && m_slots[i].item->getLogisticsComp()->id == m_slots[j].item->getLogisticsComp()->id)
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
            if (m_slots[i].item == nullptr && m_slots[j].item != nullptr)
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
            if (m_slots[i].item == nullptr && first)
            {
                m_slots[i].item = new Entity(*ItemHandler::getTemplate(itemID));
                m_slots[i].item->getSpriteComp()->setPosition(m_slots[i].rect.getPosition() + (sf::Vector2f(m_slotSize) / 2.f) - (m_slots[i].item->getSpriteComp()->getTextureSize() / 2.f));
                m_slots[i].infinite = true;
                m_slots[i].size = 8;
                m_slots[i].text.setString("Inf");
                first = false;            
            }

            else if (m_slots[i].item && m_slots[i].item->getLogisticsComp()->id == itemID && first)
            {
                m_slots[i].infinite = true;
                m_slots[i].size = 8;
                m_slots[i].text.setString("Inf");
                first = false;
            }

            else if (m_slots[i].item && m_slots[i].item->getLogisticsComp()->id == itemID)
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
                m_slots[i].item != nullptr &&
                !(m_slots[i].item->getLogisticsComp()->id == itemID && m_slots[i].item->getLogisticsComp()->stackLimit > m_slots[i].size))
            {
                i++;
            }

            if (i < ITEM_SLOT_COUNT)
            {
                if (m_slots[i].item)
                {
                    int stack = std::min(amount, m_slots[i].item->getLogisticsComp()->stackLimit - m_slots[i].size);
                    m_slots[i].size += stack;
                    amount -= stack;
                    m_slots[i].text.setString(std::to_string(m_slots[i].size));
                }

                else
                {
                    m_slots[i].item = new Entity(*ItemHandler::getTemplate(itemID));
                    m_slots[i].item->getSpriteComp()->setPosition(m_slots[i].rect.getPosition() + (sf::Vector2f(m_slotSize) / 2.f) - (m_slots[i].item->getSpriteComp()->getTextureSize() / 2.f));
                    int stack = std::min(amount, m_slots[i].item->getLogisticsComp()->stackLimit - m_slots[i].size);
                    m_slots[i].size += stack;
                    amount -= stack;
                    m_slots[i].text.setString(std::to_string(m_slots[i].size));
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
        if (m_slots[i].item != nullptr && m_slots[i].item->getLogisticsComp()->id == itemID)
        {
            int itemsTaken = std::min(amount, m_slots[i].size);
            tally += itemsTaken;

            m_slots[i].size -= itemsTaken;
            amount -= itemsTaken;
            m_slots[i].text.setString(std::to_string(m_slots[i].size));

            if (m_slots[i].size <= 0)
            {
                delete m_slots[i].item;
                m_slots[i].item = nullptr;
            }
        }
    }

    return tally;
}

void InventoryHandler::removeAt(int slot)
{
    delete m_slots[slot].item;
    m_slots[slot].item = nullptr;
    m_slots[slot].infinite = false;
    m_slots[slot].size = 0;
}

bool InventoryHandler::mergeStacks(int to, int from)
{
    if (!m_slots[to].item || !m_slots[from].item || m_slots[to].item->getLogisticsComp()->id != m_slots[from].item->getLogisticsComp()->id)
        return false;

    int avaliableSpace = m_slots[to].item->getLogisticsComp()->stackLimit - m_slots[to].size;

    m_slots[to].size += std::min(m_slots[from].size, avaliableSpace);
    m_slots[from].size -= std::min(m_slots[from].size, avaliableSpace);

    m_slots[to].text.setString(std::to_string(m_slots[to].size));
    m_slots[from].text.setString(std::to_string(m_slots[from].size));

    if (m_slots[from].size <= 0)
    {
        delete m_slots[from].item;
        m_slots[from].item = nullptr;
        return true;
    }

    return false;
}

void InventoryHandler::swapItems(int a, int b)
{
    if (m_slots[a].item)
        m_slots[a].item->getSpriteComp()->setPosition(m_slots[b].rect.getPosition() + (sf::Vector2f(m_slotSize) / 2.f) - (m_slots[a].item->getSpriteComp()->getTextureSize() / 2.f));

    if (m_slots[b].item)
        m_slots[b].item->getSpriteComp()->setPosition(m_slots[a].rect.getPosition() + (sf::Vector2f(m_slotSize) / 2.f) - (m_slots[b].item->getSpriteComp()->getTextureSize() / 2.f));

    std::swap(m_slots[a].item, m_slots[b].item);
    std::swap(m_slots[a].size, m_slots[b].size);
    std::swap(m_slots[a].infinite, m_slots[b].infinite);

    std::string temp = m_slots[a].text.getString();
    m_slots[a].text.setString(m_slots[b].text.getString());
    m_slots[b].text.setString(temp);

    if (a < QUICKSLOT_COUNT || b < QUICKSLOT_COUNT)
        updateQuickslotSprites();

}

void InventoryHandler::updateQuickslotSprites()
{
    for (int i = 0; i < QUICKSLOT_COUNT; i++)
    {
        if (m_slots[i].item)
        {
            m_quickslots[i].sprite.setTexture(*m_slots[i].item->getSpriteComp()->getTexture(), true);
            m_quickslots[i].sprite.setPosition(m_quickslots[i].rect.getPosition() + (sf::Vector2f(m_slotSize) / 2.f) - (m_slots[i].item->getSpriteComp()->getTextureSize() / 2.f));

            m_quickslots[i].text.setString(m_slots[i].text.getString());
        }
    }
}

void InventoryHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_quickslotsHidden)
    {
        for (int i = 0; i < QUICKSLOT_COUNT; i++)
        {
            target.draw(m_quickslots[i].rect, states);
            if (m_slots[i].item)
            {
                target.draw(m_quickslots[i].sprite, states);
                target.draw(m_quickslots[i].text, states);
            }
        }
    }

    if (m_inventoryOpen)
    {
        for (int i = 0; i < ITEM_SLOT_COUNT; i++)
        {
            target.draw(m_slots[i].rect, states);
            if (m_slots[i].item)
            {
                target.draw(*m_slots[i].item->getSpriteComp(), states);
                target.draw(m_slots[i].text, states);
            }
        }

        if (m_currentToolTip != -1)
            target.draw(m_toolTip, states);
    }
}
