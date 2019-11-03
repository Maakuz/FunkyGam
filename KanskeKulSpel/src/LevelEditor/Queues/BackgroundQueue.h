#pragma once
#include "SFML\Graphics.hpp"
#include <vector>

class BackgroundQueue
{
public:
    static BackgroundQueue & get()
    {
        static BackgroundQueue queue;
        return queue;
    }
    virtual ~BackgroundQueue() {};

    void queue(sf::RectangleShape rect)
    {
        rects.push_back(rect);
    }

    void clear()
    {
        rects.clear();
    }

    std::vector<sf::RectangleShape>& getQueue()
    {
        return rects;
    }

private:
    // Todo: change this to sprites when the time comes
    std::vector<sf::RectangleShape> rects;
};