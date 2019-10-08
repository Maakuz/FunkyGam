#pragma once
struct Line
{
    sf::Vector2f p1;
    sf::Vector2f p2;


    Line(sf::Vector2f p1, sf::Vector2f p2)
    {
        this->p1 = p1;
        this->p2 = p2;
    }
};