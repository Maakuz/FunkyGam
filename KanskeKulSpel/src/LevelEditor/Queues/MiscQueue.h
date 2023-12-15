#pragma once
#include "SFML/Graphics.hpp"
#include <vector>

class MiscQueue
{
public:
    struct Item
    {
        Item(sf::Drawable* ptr, bool isInterface)
        {
            this->ptr = ptr;
            this->isInterface = isInterface;
        }
        sf::Drawable* ptr;
        bool isInterface;
    };

    static MiscQueue& get()
    {
        static MiscQueue queue;
        return queue;
    }
    virtual ~MiscQueue() 
    {
        for (Item& item : items)
            if (item.ptr != nullptr)
                delete item.ptr;
    };

    void queue(sf::RectangleShape rect, bool isInterface = false)
    {
        items.push_back(Item(new sf::RectangleShape(rect), isInterface));
    }

    void queue(sf::Text text, bool isInterface = false)
    {
        items.push_back(Item(new sf::Text(text), isInterface));
    }

    void clear()
    {
        for (Item& item : items)
            delete item.ptr;

        items.clear();
    }

    std::vector<Item>& getQueue()
    {
        return items;
    }

private:
    std::vector<Item> items;
};