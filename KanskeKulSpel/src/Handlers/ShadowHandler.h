#pragma once
#include "SFML/Graphics.hpp"
#include <vector>

class ShadowHandler
{
public:
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

    ShadowHandler();
    ~ShadowHandler() {};

    void generateShadowMap();

    static void queueLine(Line line);

private:
    struct PointOnLine 
    {
        sf::Vector2f p;
        Line* parent;

        PointOnLine(sf::Vector2f p, Line* parent)
        {
            this->p = p;
            this->parent = parent;
        }
    };

    sf::RenderTexture shadowMap;
    std::vector<sf::ConvexShape> triangles;
    std::vector<PointOnLine> points;

    static std::vector<Line> lines;
};
