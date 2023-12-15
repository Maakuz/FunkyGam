#pragma once
#include "SFML/Graphics.hpp"
#include <vector>

class BackgroundQueue
{
public:
    static BackgroundQueue & get()
    {
        static BackgroundQueue queue;
        return queue;
    }
    virtual ~BackgroundQueue() 
    {
        for (sf::Drawable*& obj : objects)
            delete obj;
    };

    void queue(sf::RectangleShape rect)
    {
        objects.push_back(new sf::RectangleShape(rect));
    }

    void queue(sf::Text text)
    {
        objects.push_back(new sf::Text(text));
    }

    void clear()
    {
        for (sf::Drawable*& obj : objects)
            delete obj;

        objects.clear();
    }

    std::vector<sf::Drawable*>& getQueue()
    {
        return objects;
    }

private:
    std::vector<sf::Drawable*> objects;
};