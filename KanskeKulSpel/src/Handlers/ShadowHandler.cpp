#include "ShadowHandler.h"
#include "Misc/Definitions.h"
#include "Handlers/ShaderHandler.h"
#include "Imgui/imgui.h"
#include "Misc/Profiler.h"

std::vector<ShadowHandler::Line> ShadowHandler::ShadowHandler::lines;


ShadowHandler::ShadowHandler()
{
}

//Please don't look here
void ShadowHandler::generateShadowMap(sf::RenderTarget& target)
{
    for (size_t k = 0; k < LightQueue::get().getQueue().size(); k++)
    {
        Light* light = LightQueue::get().getQueue()[k];

        std::vector<Line> currentLines = this->lines;
        PROFILER_START("Shadow prep")
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

                p1.angle = atan2(p1.p.y - light->pos.y, p1.p.x - light->pos.x);
                p2.angle = atan2(p2.p.y - light->pos.y, p2.p.x - light->pos.x);

                points.push_back(p1);
                points.push_back(p2);
            }

            else
            {
                currentLines.erase(currentLines.begin() + i);
                i--;
            }
        }
        PROFILER_STOP

        PROFILER_START("sort")
        std::sort(points.begin(), points.end());
        PROFILER_STOP
#pragma endregion

        std::set<Line*> open;
        Line* closest = nullptr;

        sf::ConvexShape tri(3);
        tri.setFillColor(sf::Color::White);
        tri.setPoint(0, sf::Vector2f(light->radius, light->radius));

        PROFILER_START("basecase")
#pragma region basecase -- uses old tech but it works with reasonable fast
        //base case
        {
            sf::Vector2f dir(-1, 0);
            for (size_t i = 0; i < currentLines.size(); i++)
            {
                float t = findIntersectionPoint(light->pos, dir, currentLines[i].p1, currentLines[i].p2);

                if (abs(t + 1) > EPSYLONE)
                    open.insert(&currentLines[i]);
            }
        }
        //find closest
        if (!open.empty())
        {
            /*auto iterator = open.begin();

            auto min = iterator;


            iterator++;
            while (iterator != open.end())
            {
                sf::Vector2f minCenter = getCenterPoint((*min)->p1, (*min)->p2);
                sf::Vector2f center2 = getCenterPoint((*iterator)->p1, (*iterator)->p2);

                if (lengthSquared(center2 - light->pos) <= lengthSquared(minCenter - light->pos) + EPSYLONE)
                    min = iterator;

                iterator++;
            }

            closest = (*min);
*/
            sf::Vector2f dir(-1, 0);
            closest = this->findClosestLine(open, light->pos, dir);

            sf::Vector2f p;

            if (atan2(closest->p1.y - light->pos.y, closest->p1.x - light->pos.x) > 0)
                p = closest->p1;

            else
                p = closest->p2;


            if (atan2(p.y - light->pos.y, p.x - light->pos.x) > atan2(points[points.size() - 1].p.y - light->pos.y, points[points.size() - 1].p.x - light->pos.x)
                || p == points[points.size() - 1].p)
                tri.setPoint(1, p - light->pos + sf::Vector2f(light->radius, light->radius));

            else
            {
                if (open.size() > 1)
                {
                    tri.setPoint(1, p - light->pos + sf::Vector2f(light->radius, light->radius));
                }

                else
                {
                    sf::Vector2f dir = points[points.size() - 1].p - light->pos;
                    normalize(dir);

                    float t = findClosestIntersectionDistance(open, light->pos, dir);

                    tri.setPoint(1, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light->radius, light->radius));
                }
            }
        }
#pragma endregion

        PROFILER_STOP
        //debug
        //static float stopVal = 100;

                PROFILER_START("Iteracion: insert");
        //iteration start!
        for (size_t i = 0; i < points.size(); i++)
        {
            //debug
            //if (atan2(points[i].p.y - light->pos.y, points[i].p.x - light->pos.x) > stopVal)
              //  break;

            if (!open.count(points[i].parent))
            {
                if (closest == nullptr)
                {
                    closest = points[i].parent;
                    tri.setPoint(1, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));
                }

                else
                {
                    sf::Vector2f contenderOffset;

                    if (points[i].p == points[i].parent->p1)
                        contenderOffset = this->interpolateCorner(points[i].p, points[i].parent->p2, 0.05f);
                    else
                        contenderOffset = this->interpolateCorner(points[i].p, points[i].parent->p1, 0.05f);

                    sf::Vector2f dir = points[i].p - light->pos;
                    normalize(dir);

                    float closestContender = length(contenderOffset - light->pos);
                    float currentClosest = this->findIntersectionPoint(light->pos, dir, closest->p1, closest->p2);

                    if (closestContender < currentClosest && currentClosest >= 0)
                    {
                        float t = findClosestIntersectionDistance(open, light->pos, dir);

                        if (abs(t + 1) > EPSYLONE)
                            tri.setPoint(2, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light->radius, light->radius));

                        else
                            tri.setPoint(2, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius)); // this one is tricky, needs to be projected or someting

                        triangles.push_back(tri);

                        closest = points[i].parent;
                        tri.setPoint(1, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));
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

                    tri.setPoint(2, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));
                    triangles.push_back(tri);

                    tri.setPoint(1, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));

                    closest = nullptr;
                }

                else
                {

                    open.erase(points[i].parent);

                    if (closest == points[i].parent)
                    {
                        sf::Vector2f dir = points[i].p - light->pos;
                        normalize(dir);

                        closest = this->findClosestLine(open, light->pos, dir);

                        tri.setPoint(2, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));

                        triangles.push_back(tri);


                        float t = findClosestIntersectionDistance(open, light->pos, dir);

                        //Caring for corner case crap
                        if (i + 1 < points.size() && points[i].p == points[i + 1].p)
                            tri.setPoint(1, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));

                        else if (abs(t + 1) > EPSYLONE)
                            tri.setPoint(1, sf::Vector2f(dir.x * t, dir.y * t) + sf::Vector2f(light->radius, light->radius));

                        else
                            tri.setPoint(1, points[i].p - light->pos + sf::Vector2f(light->radius, light->radius));

                    }
                }

            }
        }
                PROFILER_STOP
        //ImGui::Begin("LightData");
        //ImGui::Text(std::string(std::to_string(light->pos.x) + ", " + std::to_string(light->pos.y)).c_str());
        //ImGui::Text(std::string(std::to_string(light->radius)).c_str());
        //ImGui::Text(std::string("Triangles: " + std::to_string(triangles.size())).c_str());
        //ImGui::SliderFloat("radians", &stopVal, -3, 3);
        //ImGui::End();

        PROFILER_START("draw triangles")
        drawShadowMap(light);

        triangles.clear();
        PROFILER_STOP

        PROFILER_START("lighting draw call")
        ShaderHandler::getShader(SHADER::lighting).setUniform("pos", light->pos);
        ShaderHandler::getShader(SHADER::lighting).setUniform("radius", light->radius);
        ShaderHandler::getShader(SHADER::lighting).setUniform("color", light->color);

        sf::RenderStates state;
        state.shader = &ShaderHandler::getShader(SHADER::lighting);
        state.blendMode = sf::BlendAdd;

        sf::Sprite sprite(light->shadowMap.getTexture());
        sprite.setPosition(light->pos - (sf::Vector2f(light->shadowMap.getSize()) / 2.f));
        target.draw(sprite, state);
        PROFILER_STOP

    }
    lines.clear();
    //We done bois
}

void ShadowHandler::drawShadowMap(Light* light)
{
    //For debugging
    /*for (size_t i = 0; i < triangles.size(); i++)
    {
        triangles[i].setFillColor(sf::Color(100, 255 * (float(i) / triangles.size()), 255 * (float(i) / triangles.size()), 255));
    }*/

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
