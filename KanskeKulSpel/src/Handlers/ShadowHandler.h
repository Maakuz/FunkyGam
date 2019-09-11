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
    };

    ShadowHandler();
    ~ShadowHandler() {};

    void generateShadowMap();

    static void queueLine(Line line);

private:
    

    sf::RenderTexture shadowMap;
    std::vector<sf::ConvexShape> triangles;
    std::vector<sf::Vector2f> points;

    static std::vector<Line> lines;
};
