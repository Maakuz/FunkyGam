#pragma once
#include "SFML/Graphics.hpp"
#include "Lighting/LightQueue.h"
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

    //There is a case where perpendicular lines lets light through that will be fixed one day
    void generateShadowMap(sf::RenderTarget& target);


    static void queueLine(Line line);

private:

    struct PointOnLine
    {
        sf::Vector2f p;
        ShadowHandler::Line* parent;
        float angle;

        PointOnLine(sf::Vector2f p, ShadowHandler::Line* parent)
        {
            this->p = p;
            this->parent = parent;
            this->angle = 0;
        }

        bool operator < (const PointOnLine& other) const
        {
            return this->angle < other.angle;
        }
    };

    std::vector<sf::ConvexShape> triangles;

    static std::vector<Line> lines;

    void drawShadowMap(Light* light);
    sf::Vector2f getCenterPoint(sf::Vector2f p1, sf::Vector2f p2);

    //if direction is not normalized ima be mad
    float findIntersectionPoint(sf::Vector2f pos, sf::Vector2f dir, sf::Vector2f p1, sf::Vector2f p2);
    //if direction is not normalized ima be mad
    float findClosestIntersectionDistance(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir);

    bool inBounds(sf::FloatRect bound, sf::Vector2f point);
    bool lineVSaabbTest(sf::FloatRect bounds, sf::Vector2f p1, sf::Vector2f p2);

    Line* findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos);

    sf::Vector2f interpolateCorner(sf::Vector2f corner, sf::Vector2f otherPoint, float value);
};
