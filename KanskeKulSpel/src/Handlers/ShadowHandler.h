#pragma once
#include "SFML/Graphics.hpp"
#include "Lighting/LightQueue.h"
#include "Misc/Line.h"
#include <vector>
#include <set>

class ShadowHandler
{
public:
    ShadowHandler();
    ~ShadowHandler() {};

    void generateShadowMap(sf::RenderTarget& target, sf::Vector2f viewOffset);
    void clearLines();

    static void queueLine(Line line);

private:

    struct PointOnLine
    {
        sf::Vector2f p;
        Line* parent;
        float angle;

        PointOnLine(sf::Vector2f p, Line* parent)
        {
            this->p = p;
            this->parent = parent;
            this->angle = 0;
        }

        bool operator< (const PointOnLine& other) const
        {
            return this->angle < other.angle; //:(
        }
    };

    std::vector<sf::ConvexShape> triangles;

    static std::vector<Line> lines;

    bool debugShadows;

    void drawShadowMap(Light* light);
    sf::Vector2f getCenterPoint(sf::Vector2f p1, sf::Vector2f p2);

    //if direction is not normalized ima be mad
    float findClosestIntersectionDistance(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir);

    bool inBounds(sf::FloatRect bound, sf::Vector2f point);
    bool lineVSaabbTest(sf::FloatRect bounds, sf::Vector2f p1, sf::Vector2f p2);

    //if direction is not normalized ima be mad
    Line* findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir);
    Line* findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos);

    sf::Vector2f interpolateCorner(sf::Vector2f corner, sf::Vector2f otherPoint, float value);
};
