#include "ShadowHandler.h"
#include "Lighting/LightQueue.h"

ShadowHandler::ShadowHandler()
{
}

void ShadowHandler::generateShadowMap()
{
    lines.push_back(Line(sf::Vector2f(100, 50), sf::Vector2f(300, 50)));


    for (size_t i = 0; i < lines.size(); i++)
    {
        points.push_back(PointOnLine(lines[i].p1, &lines[i]));
        points.push_back(PointOnLine(lines[i].p2, &lines[i]));
    }
    
    sf::Vector2f lightPos = LightQueue::get().getQueue()[0].pos;

    for (size_t i = 0; i < points.size(); i++)
    {
        int min = i;
        for (size_t j = i + 1; j < points.size() - 1; j++)
        {
            if (atan2(points[min].p.y - lightPos.y, points[min].p.x - lightPos.x) > atan2(points[j].p.y - lightPos.y, points[j].p.x - lightPos.x))
                min = j;
        }

        if (min != i)
            std::swap(points[min], points[i]);
    }

}

void ShadowHandler::queueLine(Line line)
{
    lines.push_back(line);
}
