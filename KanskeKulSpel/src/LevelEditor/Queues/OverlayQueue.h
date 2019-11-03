#pragma once
#include "SFML\Graphics.hpp"
#include <vector>

class OverlayQueue
{
public:
    static OverlayQueue & get()
    {
        static OverlayQueue queue;
        return queue;
    }
    virtual ~OverlayQueue() {};

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