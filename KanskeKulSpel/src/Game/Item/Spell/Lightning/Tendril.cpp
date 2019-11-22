#include "Tendril.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Handlers/TextureHandler.h"

Tendril::Tendril(float thickness, int splits, int sway, float peakHeight)
{
    this->color = sf::Color::White;
    this->sway = sway;
    this->peakHeight = peakHeight;
    this->lines.setPrimitiveType(sf::Quads);
    this->texture = TextureHandler::get().getTexture(28); // 28
}

void Tendril::generateLightning(sf::Vector2f start, sf::Vector2f end, float thickness, int splits)
{
    lines.clear();
    
    generateBranch(start, end, thickness, splits);
}


void Tendril::generateLightningTree(sf::Vector2f start, sf::Vector2f end, float thickness, int splits , int min, int max, int angle)
{
    lines.clear();

    generateLightningTreeRec(start, end, thickness, splits, min, max, angle);
}

void Tendril::generateLightningTreeRec(sf::Vector2f start, sf::Vector2f end, float thickness, int splits, int min, int max, int angle)
{
    if (max <= min)
        max = min + 1;

    int forks = min + rand() % (max - min);

    std::vector<sf::Vector2f> points = generateBranch(start, end, thickness, splits);

    for (int i = 0; i < forks; i++)
    {
        if (i % 1 == 0)
            angle = -angle;

        int pos = rand() % points.size();

        sf::Vector2f newEnd = rotateBy(angle, end, points[pos]);

        int newMin = min / 2;
        int newMax = max / 2;
        int newThickness = thickness * 0.7;
        int newSplits = splits * 0.7;
        generateLightningTreeRec(points[pos], newEnd, newThickness, newSplits, newMin, newMax, angle);
    }
}

std::vector<sf::Vector2f> Tendril::generateBranch(sf::Vector2f start, sf::Vector2f end, float thickness, int splits)
{
    std::vector<sf::Vector2f> points;
    points.push_back(start);

    sf::Vector2f dir = end - start;
    sf::Vector2f normal = sf::Vector2f(dir.y, -dir.x);
    normalize(normal);
    float distance = length(dir);

    std::vector<float> positions;
    positions.push_back(0.f);

    for (int i = 0; i < splits; i++)
        positions.push_back((rand() % 100) / 100.f);

    std::sort(positions.begin(), positions.end());

    if (sway == 0)
        sway = 1;

    float jaggyness = peakHeight / sway;

    sf::Vector2f prev = start;
    float prevDisplacement = 0;
    for (int i = 1; i < positions.size(); i++)
    {
        float scale = (distance * jaggyness) * (positions[i] - positions[i - 1]);

        float env = 0;
        if (positions[i] > 0.95f)
            env = 20 * (1 - positions[i]);

        else
            env = 1;

        float displacement = (rand() % (sway * 2)) - sway;
        displacement -= (displacement - prevDisplacement) * (1 - scale);
        displacement *= env;

        sf::Vector2f point = start + (positions[i] * dir) + (displacement * normal);

        addLine(prev, point, thickness);
        prev = point;
        prevDisplacement = displacement;

        points.push_back(point);
    }

    addLine(prev, end, thickness);

    points.push_back(end);
    return points;
}

void Tendril::addLine(sf::Vector2f p1, sf::Vector2f p2, float thickness)
{
    sf::Vector2f dir = p2 - p1;
    normalize(dir);
    sf::Vector2f right(dir.y, -dir.x);
    float texWidth = texture->getSize().x;
    float texHeight = texture->getSize().y;
    float scale = thickness / texture->getSize().y;

    sf::Vertex vertex;

    vertex.color = color;

    //Corner
    vertex.texCoords = sf::Vector2f(0, 0);
    vertex.position = p1 + (dir * -(texWidth * scale)) + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p1 + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p1 + (right * -thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(0, texHeight);
    vertex.position = p1 + (dir * -(texWidth * scale)) + (right * -thickness / 2.f);
    lines.append(vertex);

    //middle
    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p1 + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p2 + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p2 + (right * -thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p1 + (right * -thickness / 2.f);
    lines.append(vertex);

    //Corner2
        //Corner
    vertex.texCoords = sf::Vector2f(0, 0);
    vertex.position = p2 + (dir * (texWidth * scale)) + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p2 + (right * thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p2 + (right * -thickness / 2.f);
    lines.append(vertex);

    vertex.texCoords = sf::Vector2f(0, texHeight);
    vertex.position = p2 + (dir * (texWidth * scale)) + (right * -thickness / 2.f);
    lines.append(vertex);
}

void Tendril::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = this->texture;
    target.draw(lines, states);
}
