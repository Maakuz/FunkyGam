#include "ShadowHandler.h"
#include "Lighting/LightQueue.h"
#include "Misc/Definitions.h"
#include "Imgui/imgui.h"

std::vector<ShadowHandler::Line> ShadowHandler::ShadowHandler::lines;

struct PointOnLine
{
    sf::Vector2f p;
    ShadowHandler::Line* parent;

    PointOnLine(sf::Vector2f p, ShadowHandler::Line* parent)
    {
        this->p = p;
        this->parent = parent;
    }
};


ShadowHandler::ShadowHandler()
{
}

//Please don't look here
void ShadowHandler::generateShadowMap(sf::RenderTarget& target, sf::RenderStates states)
{
    Light light = LightQueue::get().getQueue()[0];
    shadowMap.create(light.radius * 2, light.radius * 2);

    //For loop this perhaps
    sf::Vector2f topRight(light.pos.x + light.radius, light.pos.y - light.radius);
    sf::Vector2f topLeft(light.pos.x - light.radius, light.pos.y - light.radius);
    sf::Vector2f bottomLeft(light.pos.x - light.radius, light.pos.y + light.radius);
    sf::Vector2f bottomRight(light.pos.x + light.radius, light.pos.y + light.radius);

    sf::FloatRect bounds(topLeft, sf::Vector2f(light.radius * 2, light.radius * 2));
    

    lines.push_back(Line(topRight, bottomRight));
    lines.push_back(Line(bottomRight, bottomLeft));
    lines.push_back(Line(bottomLeft, topLeft));
    lines.push_back(Line(topLeft, topRight));

    //testlines
   //lines.push_back(Line(sf::Vector2f(500, 300), sf::Vector2f(800, 300)));
   //lines.push_back(Line(sf::Vector2f(1000, 0), sf::Vector2f(1000, 600)));
   //lines.push_back(Line(sf::Vector2f(900, 200), sf::Vector2f(900, 300)));

#pragma region prepping lines
    std::vector<PointOnLine> points;
    for (size_t i = 0; i < lines.size(); i++)
    {
        //clamp them to light rect or discard if outside
        if (this->inBounds(bounds, lines[i].p1) || this->inBounds(bounds, lines[i].p2))
        {
            lines[i].p1.x = std::min(bottomRight.x, std::max(topLeft.x, lines[i].p1.x));
            lines[i].p1.y = std::min(bottomRight.y, std::max(topLeft.y, lines[i].p1.y));

            lines[i].p2.x = std::min(bottomRight.x, std::max(topLeft.x, lines[i].p2.x));
            lines[i].p2.y = std::min(bottomRight.y, std::max(topLeft.y, lines[i].p2.y));

            points.push_back(PointOnLine(lines[i].p1, &lines[i]));
            points.push_back(PointOnLine(lines[i].p2, &lines[i]));
        }

        else
        {
            lines.erase(lines.begin() + i);
            i--;
        }
    }


    //Insertion Sort
    for (size_t i = 0; i < points.size(); i++)
    {
        int min = i;
        for (size_t j = i + 1; j < points.size(); j++)
        {
            if (atan2(points[min].p.y - light.pos.y, points[min].p.x - light.pos.x) > atan2(points[j].p.y - light.pos.y, points[j].p.x - light.pos.x))
                min = j;
        }

        if (min != i)
            std::swap(points[min], points[i]);
    }
#pragma endregion

    std::set<Line*> open;
    Line* closest = nullptr;

    sf::ConvexShape tri(3);
    tri.setFillColor(sf::Color::White);
    tri.setPoint(0, sf::Vector2f(light.radius, light.radius));

#pragma region basecase -- uses old tech but it works with reasonable fast
    //base case
    {
        sf::Vector2f dir(-1, 0);
        for (size_t i = 0; i < lines.size(); i++)
        {
            float t = findIntersectionPoint(light.pos, dir, lines[i].p1, lines[i].p2);

            if (abs(t + 1) > EPSYLONE)
                open.insert(&lines[i]);
        }
    }
    //find closest
    if (!open.empty())
    {
        auto iterator = open.begin();

        auto min = iterator;


        iterator++;
        while (iterator != open.end())
        {
            sf::Vector2f minCenter = getCenterPoint((*min)->p1, (*min)->p2);
            sf::Vector2f center2 = getCenterPoint((*iterator)->p1, (*iterator)->p2);

            if (lengthSquared(center2 - light.pos) <= lengthSquared(minCenter - light.pos) + EPSYLONE)
                min = iterator;

            iterator++;
        }

        closest = (*min);

        sf::Vector2f p;

        if (atan2(closest->p1.y - light.pos.y, closest->p1.x - light.pos.x) > 0)
            p = closest->p1;

        else
            p = closest->p2;


        if (atan2(p.y - light.pos.y, p.x - light.pos.x) > atan2(points[points.size() - 1].p.y - light.pos.y, points[points.size() - 1].p.x - light.pos.x)
            || p == points[points.size() - 1].p)
            tri.setPoint(1, p - light.pos + sf::Vector2f(light.radius, light.radius));

        else
        {
            if (open.size() > 1)
            {
                tri.setPoint(1, p - light.pos + sf::Vector2f(light.radius, light.radius));
            }

            else
            {
                sf::Vector2f dir = points[points.size() - 1].p - light.pos;
                normalize(dir);

                float t = findClosestIntersectionDistance(open, light.pos, dir);

                tri.setPoint(1, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light.radius, light.radius));
            }
        }
    }
#pragma endregion


    //debug
    //static float stopVal = 100;

    //iteration start!
    for (size_t i = 0; i < points.size(); i++)
    {
        //debug
        //if (atan2(points[i].p.y - light.pos.y, points[i].p.x - light.pos.x) > stopVal)
          //  break;

        if (!open.count(points[i].parent))
        {

            if (closest == nullptr)
            {
                closest = points[i].parent;
                tri.setPoint(1, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));
            }

            else
            {
                sf::Vector2f contenderOffset;

                if (points[i].p == points[i].parent->p1)
                    contenderOffset = this->interpolateCorner(points[i].p, points[i].parent->p2, 0.05f);
                else
                    contenderOffset = this->interpolateCorner(points[i].p, points[i].parent->p1, 0.05f);

                sf::Vector2f dir = points[i].p - light.pos;
                normalize(dir);

                float closestContender = length(contenderOffset - light.pos);
                float currentClosest = this->findIntersectionPoint(light.pos, dir, closest->p1, closest->p2);

                if (closestContender < currentClosest && currentClosest >= 0)
                {
                    float t = findClosestIntersectionDistance(open, light.pos, dir);

                    if (abs(t + 1) > EPSYLONE)
                        tri.setPoint(2, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light.radius, light.radius));

                    else
                        tri.setPoint(2, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius)); // this one is tricky, needs to be projected or someting

                    triangles.push_back(tri);

                    closest = points[i].parent;
                    tri.setPoint(1, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));
                }
            }

            open.insert(points[i].parent);

        }

        else
        {
            //ooh i dont know bout this one Bobby
            if (open.size() == 1)
            {
                open.erase(points[i].parent);

                tri.setPoint(2, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));
                triangles.push_back(tri);

                tri.setPoint(1, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));

                closest = nullptr;
            }

            else
            {
                open.erase(points[i].parent);

                if (closest == points[i].parent)
                {
                    sf::Vector2f dir = points[i].p - light.pos;
                    normalize(dir);

                    closest = this->findClosestLine(open, light.pos, dir);

                    tri.setPoint(2, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));

                    triangles.push_back(tri);


                    float t = findClosestIntersectionDistance(open, light.pos, dir);

                    //Caring for corner case crap
                    if (i+1 < points.size() && points[i].p == points[i + 1].p)
                        tri.setPoint(1, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));

                    else if (abs(t + 1) > EPSYLONE)
                        tri.setPoint(1, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light.radius, light.radius));

                    else
                        tri.setPoint(1, points[i].p - light.pos + sf::Vector2f(light.radius, light.radius));

                }
            }
        }
    }

    //ImGui::Begin("LightData");
    //ImGui::Text(std::string(std::to_string(light.pos.x) + ", " + std::to_string(light.pos.y)).c_str());
    //ImGui::Text(std::string(std::to_string(light.radius)).c_str());
    //ImGui::Text(std::string("Triangles: " + std::to_string(triangles.size())).c_str());
    //ImGui::SliderFloat("radians", &stopVal, -3, 3);
    //ImGui::End();

    drawShadowMap();

    lines.clear();
    triangles.clear();

    sf::Sprite test(shadowMap.getTexture());
    test.setPosition(light.pos - (sf::Vector2f(shadowMap.getSize()) / 2.f));
    target.draw(test, states);
    //We done bois

}

void ShadowHandler::drawShadowMap()
{
    //For debugging
    /*for (size_t i = 0; i < triangles.size(); i++)
    {
        triangles[i].setFillColor(sf::Color(100, 255 * (float(i) / triangles.size()), 255 * (float(i) / triangles.size()), 255));
    }*/

    shadowMap.clear(sf::Color::Transparent);
    for (auto const & polies : this->triangles)
    {
        shadowMap.draw(polies);
    }
    shadowMap.display();


}

sf::Vector2f ShadowHandler::getCenterPoint(sf::Vector2f p1, sf::Vector2f p2)
{
    return (p1 + p2) / 2.f;
}

float ShadowHandler::findIntersectionPoint(sf::Vector2f pos, sf::Vector2f dir, sf::Vector2f p1, sf::Vector2f p2)
{
    sf::Vector2f v1 = pos - p1;
    sf::Vector2f v2 = p2 - p1;
    sf::Vector2f v3 = sf::Vector2f(-dir.y, dir.x);
    float dot = (v2.x * v3.x) + (v2.y * v3.y);

    float t1 = -1;
    float t2 = -1;

    if (abs(dot) > EPSYLONE)
    {
        //crioss
        t1 = ((v2.x * v1.y) - (v2.y * v1.x)) / dot;
        //dot
        t2 = ((v1.x * v3.x) + (v1.y * v3.y)) / dot;

        if (t1 < 0.f || (t2 + EPSYLONE < 0.f || t2 > 1.f + EPSYLONE))
        {
            t1 = -1;
        }
    }

    return t1;
}

float ShadowHandler::findClosestIntersectionDistance(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir)
{

    auto iterator = openList.begin();
    auto min = iterator;
    float tMin = this->findIntersectionPoint(pos, dir, (*min)->p1, (*min)->p2);

    iterator++; //might crash if only one wall
    while (iterator != openList.end())
    {

        float t = this->findIntersectionPoint(pos, dir, (*iterator)->p1, (*iterator)->p2);

        if (abs(t + 1) > EPSYLONE)
            if (t < tMin)
                tMin = t;

        iterator++;
    }
    return tMin;
}

bool ShadowHandler::inBounds(sf::FloatRect bound, sf::Vector2f point)
{
    return 
        bound.left - EPSYLONE < point.x &&
        bound.top - EPSYLONE < point.y && 
        bound.left + bound.width + EPSYLONE > point.x &&
        bound.top + bound.height + EPSYLONE > point.y;
}

//might crash if only one wall
ShadowHandler::Line* ShadowHandler::findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir)
{
    auto iterator = openList.begin();
    auto min = iterator;
    float tMin = this->findIntersectionPoint(pos, dir, (*min)->p1, (*min)->p2);

    iterator++; //might crash if only one wall
    
    if (tMin < 0 && iterator == openList.end())
            return nullptr;

    while (iterator != openList.end())
    {

        float t = this->findIntersectionPoint(pos, dir, (*iterator)->p1, (*iterator)->p2);

        if (abs(t + 1) > EPSYLONE)
            if (t < tMin)
            {
                min = iterator;
                tMin = t;
            }

        iterator++;
    }
    return (*min);
}

sf::Vector2f ShadowHandler::interpolateCorner(sf::Vector2f corner, sf::Vector2f otherPoint, float value)
{
    sf::Vector2f point;
    point.x = corner.x *(1 - value) + otherPoint.x * value;
    point.y = corner.y *(1 - value) + otherPoint.y * value;
    return point;
}

void ShadowHandler::queueLine(Line line)
{
    lines.push_back(line);
}
