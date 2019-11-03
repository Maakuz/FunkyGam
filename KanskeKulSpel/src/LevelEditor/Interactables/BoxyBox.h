#pragma once
#include "SFML\Graphics.hpp"

class BoxyBox
{
public:
    BoxyBox(float x, float y, float width, float height, float borderSize = 5);
    virtual ~BoxyBox() {};

    //void setBorderColor(sf::Color color, sf::Color color2);
    void setBorderColor(sf::Color color);
    //void setColor(sf::Color color, sf::Color color2);
    void setColor(sf::Color color);

    sf::RectangleShape getRect() { return rect; };
    bool contains(int x, int y) const;

private:

    sf::RectangleShape rect;
};