#include "ShadowHandler.h"
#include "Lighting/LightQueue.h"
#include <set>

std::vector<ShadowHandler::Line> ShadowHandler::ShadowHandler::lines;

ShadowHandler::ShadowHandler()
{
}

void ShadowHandler::generateShadowMap(sf::RenderTarget& target, sf::RenderStates states)
{
    lines.push_back(Line(sf::Vector2f(100, 50), sf::Vector2f(300, 50)));


    for (size_t i = 0; i < lines.size(); i++)
    {
        points.push_back(PointOnLine(lines[i].p1, &lines[i]));
        points.push_back(PointOnLine(lines[i].p2, &lines[i]));
    }

    lines.clear();
    
    Light light = LightQueue::get().getQueue()[0];

    for (size_t i = 0; i < points.size(); i++)
    {
        int min = i;
        for (size_t j = i + 1; j < points.size() - 1; j++)
        {
            if (atan2(points[min].p.y - light.pos.y, points[min].p.x - light.pos.x) > atan2(points[j].p.y - light.pos.y, points[j].p.x - light.pos.x))
                min = j;
        }

        if (min != i)
            std::swap(points[min], points[i]);
    }



    std::set<Line*> open;
    Line* closest = nullptr;

    sf::ConvexShape tri(3);
    tri.setFillColor(sf::Color::White);
    tri.setPoint(0, light.pos);

    for (size_t i = 0; i < points.size(); i++)
    {
        if (!open.count(points[i].parent))
        {
            open.insert(points[i].parent);
            if (closest == nullptr)
            {
                closest = points[i].parent;
                tri.setPoint(1, points[i].p);

            }

            else
            {
                sf::Vector2f closestCenter = closest->p1 - (closest->p2 / 2.f);
                sf::Vector2f newCenter = points[i].parent->p1 - (points[i].parent->p2 / 2.f);

                if ((newCenter - light.pos).x + (newCenter - light.pos).y < (closestCenter - light.pos).x + (closestCenter - light.pos).y)
                {
                    tri.setPoint(2, points[i].p); // this one is tricky, needs to be projected or someting
                    triangles.push_back(tri);

                    closest = points[i].parent;
                    tri.setPoint(1, points[i].p);
                }
            }
        }

        else
        {
            open.erase(points[i].parent);
            if (open.empty())
                closest = nullptr;

            else
            {
                if (closest == points[i].parent)
                {
                    auto iterator = open.begin();

                    auto min = iterator;

                    iterator++; //might crash if only one wall
                    while (iterator != open.end())
                    {
         
                        sf::Vector2f minCenter = (*min)->p1 - ((*min)->p2 / 2.f);
                        sf::Vector2f center2 = (*iterator)->p1 - ((*iterator)->p2 / 2.f);

                        if ((center2 - light.pos).x + (center2 - light.pos).y < (minCenter - light.pos).x + (minCenter - light.pos).y)
                            min = iterator;

                        iterator++;
                    }

                    tri.setPoint(2, points[i].p); // this one is tricky, needs to be projected or someting
                    triangles.push_back(tri);
                    closest = (*min);
                    tri.setPoint(1, points[i].p);
                }
            }
        }
    }

    shadowMap.create(light.radius, light.radius);
    drawShadowMap();
    //We done bois

    //test to see what even
    sf::Sprite test(shadowMap.getTexture());
    target.draw(test, states);

}

void ShadowHandler::drawShadowMap()
{
    shadowMap.clear(sf::Color::Black);
    for (auto const & polies : this->triangles)
    {
        shadowMap.draw(polies);
    }
    shadowMap.display();
}

void ShadowHandler::queueLine(Line line)
{
    lines.push_back(line);
}
