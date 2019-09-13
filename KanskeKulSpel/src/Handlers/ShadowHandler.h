#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <set>

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

    void generateShadowMap(sf::RenderTarget& target, sf::RenderStates states);


    static void queueLine(Line line);

private:

    void drawShadowMap();
    sf::Vector2f getCenterPoint(sf::Vector2f p1, sf::Vector2f p2);

    //if direction is not normalized ima be mad
    float findIntersectionPoint(sf::Vector2f pos, sf::Vector2f dir, sf::Vector2f p1, sf::Vector2f p2);
    //if direction is not normalized ima be mad
    float findClosestIntersection(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir);


    sf::RenderTexture shadowMap;
    std::vector<sf::ConvexShape> triangles;

    static std::vector<Line> lines;

    //These might get their own file later
    float lengthSquared(sf::Vector2f vec) const
    {
        return abs(vec.x * vec.x) + abs(vec.y * vec.y);
    }

    float length(sf::Vector2f vec) const
    {
        return sqrt(lengthSquared(vec));
    }

    void normalize(sf::Vector2f& vec)
    {
        float l = length(vec);
        vec.x = vec.x / l;
        vec.y = vec.y / l;
    }
};
