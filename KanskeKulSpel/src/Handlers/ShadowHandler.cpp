#include "ShadowHandler.h"
#include "Misc/Definitions.h"
#include "Handlers/ShaderHandler.h"
#include "Imgui/imgui.h"
#include "Misc/Profiler.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"

std::vector<Line> ShadowHandler::lines;


ShadowHandler::ShadowHandler()
{
    debugShadows = false;

    ConsoleWindow::get().addCommand("debugShadows", [&](Arguments args)->std::string 
        {
            if (args.empty())
                return "Argument 0 or 1 missing";

            else
            {
                try
                {
                    int booly = std::stoi(args[0]);
                    debugShadows = booly;
                    return "Command worked";
                }
                catch (const std::exception&)
                {
                    return "Not valid number";
                }
            }

            return "error";
        });
}

//Please don't look here
void ShadowHandler::generateShadowMap(sf::RenderTarget& target)
{
    for (size_t k = 0; k < LightQueue::get().getQueue().size(); k++)
    {
        Light* light = LightQueue::get().getQueue()[k];

        std::vector<Line> currentLines = this->lines;
        //For loop this perhaps
        sf::Vector2f topRight(light->pos.x + light->radius, light->pos.y - light->radius);
        sf::Vector2f topLeft(light->pos.x - light->radius, light->pos.y - light->radius);
        sf::Vector2f bottomLeft(light->pos.x - light->radius, light->pos.y + light->radius);
        sf::Vector2f bottomRight(light->pos.x + light->radius, light->pos.y + light->radius);

        sf::FloatRect bounds(topLeft, sf::Vector2f(light->radius * 2, light->radius * 2));


        currentLines.push_back(Line(topRight, bottomRight));
        currentLines.push_back(Line(bottomRight, bottomLeft));
        currentLines.push_back(Line(bottomLeft, topLeft));
        currentLines.push_back(Line(topLeft, topRight));

        //testlines
        //lines.push_back(Line(sf::Vector2f(500, 300), sf::Vector2f(800, 300)));
        //lines.push_back(Line(sf::Vector2f(1000, 0), sf::Vector2f(1000, 600)));
        //lines.push_back(Line(sf::Vector2f(900, 200), sf::Vector2f(900, 300)));
        //
        //Nu kör vi en diagonal jäkel helst inte
        //lines.push_back(Line(sf::Vector2f(800, 200), sf::Vector2f(900, 300)));


#pragma region prepping lines
        std::vector<PointOnLine> points;
        for (size_t i = 0; i < currentLines.size(); i++)
        {
            //clamp them to light rect or discard if outside, will only work with aabbs for now
            if (this->inBounds(bounds, currentLines[i].p1)
                || this->inBounds(bounds, currentLines[i].p2)
                || this->lineVSaabbTest(bounds, currentLines[i].p1, currentLines[i].p2))
            {
                currentLines[i].p1.x = std::min(bottomRight.x, std::max(topLeft.x, currentLines[i].p1.x));
                currentLines[i].p1.y = std::min(bottomRight.y, std::max(topLeft.y, currentLines[i].p1.y));

                currentLines[i].p2.x = std::min(bottomRight.x, std::max(topLeft.x, currentLines[i].p2.x));
                currentLines[i].p2.y = std::min(bottomRight.y, std::max(topLeft.y, currentLines[i].p2.y));

                PointOnLine p1(currentLines[i].p1, &currentLines[i]);
                PointOnLine p2(currentLines[i].p2, &currentLines[i]);

                p1.angle = atan2(p1.p.y - light->pos.y, p1.p.x - light->pos.x);// *180 / 3.14;
                p2.angle = atan2(p2.p.y - light->pos.y, p2.p.x - light->pos.x);// *180 / 3.14;

                points.push_back(p1);
                points.push_back(p2);
            }

            else
            {
                currentLines.erase(currentLines.begin() + i);
                i--;
            }
        }

        std::sort(points.begin(), points.end());
#pragma endregion
        std::set<Line*> open;
        Line* closest = nullptr;

        sf::ConvexShape tri(3);
        tri.setFillColor(sf::Color::White);
        sf::Vector2f origo = sf::Vector2f(light->radius, light->radius);
        tri.setPoint(0, origo);

        //starting point
        sf::Vector2f firstPos;
        {
            sf::Vector2f dir = points[points.size() - 1].p - light->pos;
            normalize(dir);

            for (size_t i = 0; i < currentLines.size(); i++)
            {
                float t = findIntersectionPoint(light->pos, dir, currentLines[i].p1, currentLines[i].p2);
                float t2 = findIntersectionPoint(light->pos, sf::Vector2f(-1, 0), currentLines[i].p1, currentLines[i].p2);

                if (abs(t + 1) > EPSYLONE&& abs(t2 + 1) > EPSYLONE)
                    open.insert(&currentLines[i]);
            }


            sf::Vector2f p1;
            closest = findClosestLine(open, light->pos, dir);
            float t = findIntersectionPoint(light->pos, dir, closest->p1, closest->p2);
            p1 = origo + (dir * t);
            tri.setPoint(1, p1);
            firstPos = p1;
        }

        std::vector<ShadowHandler::PointOnLine>::iterator point = points.begin();
        PointOnLine* closestPoint = nullptr;
        while (point < points.end())
        {
            closestPoint = nullptr;

            //Open or close all with the same angle
            do 
            {
                if (open.count(point->parent))
                {
                    open.erase(point->parent);
                    if (point->parent == closest)
                    {
                        closestPoint = point._Ptr;
                    }
                }

                else
                    open.insert(point->parent);

                point++;
            } while (point < points.end() && abs((point - 1)->angle - point->angle) < EPSYLONE);

            //do stuff
            sf::Vector2f dir = (point - 1)->p - light->pos;
            normalize(dir);
            Line* newClosest = findClosestLine(open, light->pos, dir);

            if (closestPoint)
            {
                sf::Vector2f p2 = origo + (closestPoint->p - light->pos);
                tri.setPoint(2, p2);
                triangles.push_back(tri);

                sf::Vector2f p1;
                float t = findIntersectionPoint(light->pos, dir, newClosest->p1, newClosest->p2);

                p1 = origo + (dir * t);
                tri.setPoint(1, p1);

                closestPoint = nullptr;
            }

            else if (newClosest != closest)
            {
                sf::Vector2f p2;
                float t = findIntersectionPoint(light->pos, dir, closest->p1, closest->p2);

                p2 = origo + (dir * t);
                tri.setPoint(2, p2);
                triangles.push_back(tri);

                sf::Vector2f p1;
                t = findIntersectionPoint(light->pos, dir, newClosest->p1, newClosest->p2);
                p1 = origo + (dir * t);
                tri.setPoint(1, p1);

            }

            closest = newClosest;
        }
        //End case
        tri.setPoint(2, firstPos);
        triangles.push_back(tri);

        if (debugShadows)
        {
            ImGui::Begin("LightData");
            ImGui::Text(std::string(std::to_string(light->pos.x) + ", " + std::to_string(light->pos.y)).c_str());
            ImGui::Text(std::string(std::to_string(light->radius)).c_str());
            ImGui::Text(std::string("Triangles: " + std::to_string(triangles.size())).c_str());
            ImGui::SliderFloat("radians", &stopVal, -3, 3);
            ImGui::End();
        }

        drawShadowMap(light);

        triangles.clear();

        ShaderHandler::getShader(SHADER::lighting).setUniform("pos", light->pos);
        ShaderHandler::getShader(SHADER::lighting).setUniform("radius", light->radius);
        ShaderHandler::getShader(SHADER::lighting).setUniform("color", light->color);

        sf::RenderStates state;
        state.shader = &ShaderHandler::getShader(SHADER::lighting);
        state.blendMode = sf::BlendAdd;

        sf::Sprite sprite(light->shadowMap.getTexture());
        sprite.setPosition(light->pos - (sf::Vector2f(light->shadowMap.getSize()) / 2.f));
        target.draw(sprite, state);


        if (debugShadows)
            target.draw(sprite);
    }
    lines.clear();
    //We done bois
}

void ShadowHandler::drawShadowMap(Light* light)
{
    //For debugging
    if (debugShadows)
    {
        for (size_t i = 0; i < triangles.size(); i++)
        {
            triangles[i].setFillColor(sf::Color(100, 255 * (float(i) / triangles.size()), 255 * (float(i) / triangles.size()), 255));
        }
    }

    light->shadowMap.clear(sf::Color::Transparent);
    for (auto const & polies : this->triangles)
    {
        light->shadowMap.draw(polies);
    }
    light->shadowMap.display();


}

sf::Vector2f ShadowHandler::getCenterPoint(sf::Vector2f p1, sf::Vector2f p2)
{
    return (p1 + p2) / 2.f;
}

float ShadowHandler::findClosestIntersectionDistance(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir)
{

    auto iterator = openList.begin();
    auto min = iterator;
    float tMin = findIntersectionPoint(pos, dir, (*min)->p1, (*min)->p2);

    iterator++; //might crash if only one wall
    while (iterator != openList.end())
    {

        float t = findIntersectionPoint(pos, dir, (*iterator)->p1, (*iterator)->p2);

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

//only works for straight lines
bool ShadowHandler::lineVSaabbTest(sf::FloatRect bounds, sf::Vector2f p1, sf::Vector2f p2)
{
    return (p1.x < bounds.left
            && p2.x > bounds.left + bounds.width
            && p1.y > bounds.top
            && p1.y < bounds.top + bounds.height
            && p2.y > bounds.top
            && p2.y < bounds.top + bounds.height)

        || (p2.x < bounds.left
            && p1.x > bounds.left + bounds.width
            && p1.y > bounds.top
            && p1.y < bounds.top + bounds.height
            && p2.y > bounds.top
            && p2.y < bounds.top + bounds.height)

        || (p1.y < bounds.top
            && p2.y > bounds.top + bounds.height
            && p1.x > bounds.left
            && p1.x < bounds.left + bounds.width
            && p2.x > bounds.left
            && p2.x < bounds.left + bounds.width)

        || (p2.y < bounds.top
            && p1.y > bounds.top + bounds.height
            && p2.x > bounds.left
            && p2.x < bounds.left + bounds.width
            && p1.x > bounds.left
            && p1.x < bounds.left + bounds.width);
}

//if direction is not normalized ima be mad
Line* ShadowHandler::findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos, sf::Vector2f dir)
{
    auto iterator = openList.begin();
    auto min = iterator;

    float tMin = findIntersectionPoint(pos, dir, (*min)->p1, (*min)->p2);

    iterator++;

    if (tMin < 0 && iterator == openList.end())
        return nullptr;

    while (iterator != openList.end())
    {
        float t = findIntersectionPoint(pos, dir, (*iterator)->p1, (*iterator)->p2);

        if (abs(t + 1) > EPSYLONE)
        {
            //More precise check needed
            if (abs(t - tMin) < EPSYLONE)
            {  
                sf::Vector2f intersection = pos + (dir * t);
                sf::Vector2f interpolatedDir;
                sf::Vector2f posArr[4] = { (*iterator)->p1, (*iterator)->p2, (*min)->p1, (*min)->p2 };
                float superT;
                float superTmin;
                
                bool resolved = false;
                for (int i = 0; i < 4 && !resolved; i++)
                {
                    interpolatedDir = interpolateCorner(intersection, posArr[i], 0.5f) - pos;
                    normalize(interpolatedDir);
                    superT = findIntersectionPoint(pos, interpolatedDir, (*iterator)->p1, (*iterator)->p2);
                    superTmin = findIntersectionPoint(pos, interpolatedDir, (*min)->p1, (*min)->p2);
                    if (superT < 0 || superTmin < 0 || abs(superT - superTmin) < EPSYLONE)
                        resolved = false;

                    else
                        resolved = true;
                }


                //if they are still the same they have earned it
                if (superT < superTmin)
                {
                    min = iterator;
                    tMin = t;
                }
            }

            else if (t < tMin)
            {
                min = iterator;
                tMin = t;
            }

        }
        iterator++;
    }
    return (*min);
}

//might crash if only one wall
Line* ShadowHandler::findClosestLine(const std::set<Line*>& openList, sf::Vector2f pos)
{
    auto iterator = openList.begin();
    auto min = iterator;
    sf::Vector2f dir;

    if (lengthSquared((*min)->p1 - pos) < lengthSquared((*min)->p2 - pos))
        dir = interpolateCorner((*min)->p1, (*min)->p2, 0.05f) - pos;

    else
        dir = interpolateCorner((*min)->p2, (*min)->p1, 0.05f) - pos;

    normalize(dir);

    float tMin = findIntersectionPoint(pos, dir, (*min)->p1, (*min)->p2);

    iterator++;
    
    if (tMin < 0 && iterator == openList.end())
            return nullptr;

    while (iterator != openList.end())
    {
        if (lengthSquared((*iterator)->p1 - pos) < lengthSquared((*iterator)->p2 - pos))
            dir = interpolateCorner((*iterator)->p1, (*iterator)->p2, 0.05f) - pos;

        else
            dir = interpolateCorner((*iterator)->p2, (*iterator)->p1, 0.05f) - pos;

        normalize(dir);

        float t = findIntersectionPoint(pos, dir, (*iterator)->p1, (*iterator)->p2);

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
