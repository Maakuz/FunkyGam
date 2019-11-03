#include "Button.h"

Button::Button()
{
    sideShown = SIDES::NONE;
}

bool Button::isInside(sf::Vector2i pos) const
{
    return isInside((float)pos.x, (float)pos.y);
}

bool Button::isInside(float x, float y) const
{
    if (x >= rect[SIDES::ENTIRE].getPosition().x &&
        y >= rect[SIDES::ENTIRE].getPosition().y &&
        x < rect[SIDES::ENTIRE].getPosition().x + rect[SIDES::ENTIRE].getSize().x &&
        y < rect[SIDES::ENTIRE].getPosition().y + rect[SIDES::ENTIRE].getSize().y)
        return true;

    return false;
}

void Button::setPosition(float x, float y) //TODO: MAKE THESE FUNCTIONS NOT SO STUPID
{
    rect[SIDES::ENTIRE].setPosition(x, y);
    rect[SIDES::TOP].setPosition(x, y - rect[SIDES::TOP].getSize().y);
    rect[SIDES::RIGHT].setPosition(x + rect[SIDES::ENTIRE].getSize().x, y);
    rect[SIDES::BOTTOM].setPosition(x, y + rect[SIDES::ENTIRE].getSize().y);
    rect[SIDES::LEFT].setPosition(x - rect[SIDES::LEFT].getSize().x, y);
}

void Button::setPosition(int x, int y)
{
    setPosition(float(x), float(y));
}

void Button::setSize(float width, float height)
{
    rect[SIDES::ENTIRE].setSize({ width, height });
    rect[SIDES::TOP].setSize({ width, rect[SIDES::ENTIRE].getOutlineThickness() });
    rect[SIDES::RIGHT].setSize({ rect[SIDES::ENTIRE].getOutlineThickness(), height});
    rect[SIDES::BOTTOM].setSize({ width, rect[SIDES::ENTIRE].getOutlineThickness() });
    rect[SIDES::LEFT].setSize({ rect[SIDES::ENTIRE].getOutlineThickness(), height });
}

void Button::setOutlineThickness(float thickness)
{
    for (int i = 0; i < RECT_AMOUNT; i++)
    {
        rect[i].setOutlineThickness(thickness);
    }
    
}

void Button::setFillColor(sf::Color color)
{
    for (int i = 0; i < RECT_AMOUNT; i++)
    {
        rect[i].setFillColor(color);
    }
}

void Button::setOutlineColor(sf::Color color)
{
    for (int i = 0; i < RECT_AMOUNT; i++)
    {
        rect[i].setOutlineColor(color);
    }
}

void Button::setVisibleSide(SIDES::SIDES_VISIBLE side)
{
    sideShown = side;
}

Button::operator sf::RectangleShape()
{
    return rect[SIDES::ENTIRE];
}

Button::operator std::vector<sf::RectangleShape>()
{
    std::vector<sf::RectangleShape> rects;

    switch (sideShown)
    {
    case SIDES::ENTIRE:
        rects.push_back(rect[SIDES::ENTIRE]);
        break;

    case SIDES::TOP:
        rects.push_back(rect[SIDES::TOP]);
        break;

    case SIDES::RIGHT:
        rects.push_back(rect[SIDES::RIGHT]);
        break;

    case SIDES::BOTTOM:
        rects.push_back(rect[SIDES::BOTTOM]);
        break;

    case SIDES::LEFT:
        rects.push_back(rect[SIDES::LEFT]);
        break;


    case SIDES::TOP_LEFT:
        rects.push_back(rect[SIDES::LEFT]);
        rects.push_back(rect[SIDES::TOP]);
        break;


    case SIDES::TOP_RIGHT:
        rects.push_back(rect[SIDES::TOP]);
        rects.push_back(rect[SIDES::RIGHT]);
        break;


    case SIDES::BOTTOM_RIGHT:
        rects.push_back(rect[SIDES::BOTTOM]);
        rects.push_back(rect[SIDES::RIGHT]);
        break;


    case SIDES::BOTTOM_LEFT:
        rects.push_back(rect[SIDES::BOTTOM]);
        rects.push_back(rect[SIDES::LEFT]);
        break;

    case SIDES::TOP_LEFT_RIGHT:
        rects.push_back(rect[SIDES::LEFT]);
        rects.push_back(rect[SIDES::TOP]);
        rects.push_back(rect[SIDES::RIGHT]);
        break;


    case SIDES::RIGHT_LEFT_BOTTOM:
        rects.push_back(rect[SIDES::BOTTOM]);
        rects.push_back(rect[SIDES::RIGHT]);
        rects.push_back(rect[SIDES::LEFT]);
        break;

    case SIDES::TOP_RIGHT_BOTTOM:
        rects.push_back(rect[SIDES::TOP]);
        rects.push_back(rect[SIDES::RIGHT]);
        rects.push_back(rect[SIDES::BOTTOM]);
        break;

    case SIDES::TOP_LEFT_BOTTOM:
        rects.push_back(rect[SIDES::TOP]);
        rects.push_back(rect[SIDES::BOTTOM]);
        rects.push_back(rect[SIDES::LEFT]);
        break;

    case SIDES::TOP_BOTTOM:
        rects.push_back(rect[SIDES::TOP]);
        rects.push_back(rect[SIDES::BOTTOM]);
        break;

    case SIDES::RIGHT_LEFT:
        rects.push_back(rect[SIDES::LEFT]);
        rects.push_back(rect[SIDES::RIGHT]);
        break;

    case SIDES::NONE:
        break;
    default:
        break;
    }

    
    return rects;
}
