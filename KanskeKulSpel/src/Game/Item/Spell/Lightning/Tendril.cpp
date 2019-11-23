#include "Tendril.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/UnorderedErase.h"

Tendril::Tendril(InitGenData data)
{
    this->data = data;
    this->elapsedTime = 0;
    this->vertices.setPrimitiveType(sf::Quads);
    this->texture = TextureHandler::get().getTexture(28);
}

void Tendril::update(float dt)
{
    elapsedTime += dt;
    if (data.visibleTime != 0)
    {
        float timePercentage = elapsedTime / data.visibleTime;
        for (Branch& branch : branches)
        {
            updateBranch(&branch, timePercentage);
        }
    }

    vertices.clear();

    for (int j = 0; j < lines.size(); j++)
    {
        int alpha = lines[j][4].color.a - (data.fadeSpeed * dt);
        if (alpha <= 0)
            unordered_erase(lines, lines.begin() + j--);

        else
        {
            for (int i = 0; i < 12; i++)
            {
                if (i < 4 || i > 7) // makes corners a bit nicer
                    lines[j][i].color.a = alpha / 3.f;
                else
                    lines[j][i].color.a = alpha;

                vertices.append(lines[j][i]);
            }
        }
    }
}

void Tendril::updateBranch(Branch* branch, float time)
{
    int stop = branch->progess;
    for (int i = stop + 1; i < branch->distances.size() && branch->distances[i] < time; i++)
    {
        stop = i;
    }

    for (int i = branch->progess; i < stop; i++)
    {
        addLine(branch->points[i], branch->points[i + 1], branch->thickness);
    }

    for (int i = 0; i < branch->branchPositions.size() && branch->branchPositions[i] < stop; i++)
    {
        float localTime = branch->distances[branch->branchPositions[i]];
        updateBranch(&branch->branches[i], (time - localTime) / localTime); //not quite right but hella cool
    }

    branch->progess = stop;
}

void Tendril::generate(sf::Vector2f start, sf::Vector2f end)
{
    this->branches.clear();
    this->lines.clear();
    this->elapsedTime = 0;

    if (data.max <= 0)
        generateLightning(start, end);

    else
        generateLightningTree(start, end);
}

void Tendril::generateLightning(sf::Vector2f start, sf::Vector2f end)
{
    branches.push_back(generateBranch(start, end, data.thickness, data.splits));
}

void Tendril::generateLightningTree(sf::Vector2f start, sf::Vector2f end)
{
    generateLightningTreeRec(&branches, start, end, data.thickness, data.splits, data.min, data.max, data.angle);
}

void Tendril::generateLightningTreeRec(std::vector<Branch>* branches, sf::Vector2f start, sf::Vector2f end, float thickness, int splits, int min, int max, int angle)
{
    if (max <= min)
        max = min + 1;

    int forks = min + rand() % (max - min);

    branches->push_back(generateBranch(start, end, thickness, splits));

    for (int i = 0; i < forks; i++)
        branches->back().branchPositions.push_back(rand() % branches->back().points.size());

    std::sort(branches->back().branchPositions.begin(), branches->back().branchPositions.end());
    
    for (int i = 0; i < forks; i++)
    {
        if (i % 1 == 0)
            angle = -angle; // Maybe randomize

        int pos = branches->back().branchPositions[i];
        sf::Vector2f newEnd = rotateBy(angle, end, branches->back().points[pos]);

        int newMin = min / 2;
        int newMax = max / 2;
        int newThickness = thickness * 0.7;
        int newSplits = splits * 0.7;
        generateLightningTreeRec(&branches->back().branches, branches->back().points[pos], newEnd, newThickness, newSplits, newMin, newMax, angle);
    }
}

Tendril::Branch Tendril::generateBranch(sf::Vector2f start, sf::Vector2f end, float thickness, int splits)
{
    Branch branch;
    branch.thickness = thickness;
    branch.points.push_back(start);

    sf::Vector2f dir = end - start;
    sf::Vector2f normal = sf::Vector2f(dir.y, -dir.x);
    normalize(normal);
    float distance = length(dir);

    branch.distances.push_back(0.f);

    for (int i = 0; i < splits; i++)
        branch.distances.push_back((rand() % 100) / 100.f);

    std::sort(branch.distances.begin(), branch.distances.end());

    if (data.sway == 0)
        data.sway = 1;

    float jaggyness = data.peakHeight / data.sway;

    sf::Vector2f prev = start;
    float prevDisplacement = 0;
    for (int i = 1; i < branch.distances.size(); i++)
    {
        float scale = (distance * jaggyness) * (branch.distances[i] - branch.distances[i - 1]);

        float env = 0;
        if (branch.distances[i] > 0.95f)
            env = 20 * (1 - branch.distances[i]);

        else
            env = 1;

        float displacement = (rand() % (data.sway * 2)) - data.sway;
        displacement -= (displacement - prevDisplacement) * (1 - scale);
        displacement *= env;

        sf::Vector2f point = start + (branch.distances[i] * dir) + (displacement * normal);

        if (data.visibleTime == 0)
            addLine(prev, point, thickness);

        prev = point;
        prevDisplacement = displacement;

        branch.points.push_back(point);
    }

    if (data.visibleTime == 0)
        addLine(prev, end, thickness);

    branch.points.push_back(end);
    return branch;
}

void Tendril::addLine(sf::Vector2f p1, sf::Vector2f p2, float thickness)
{
    sf::Vector2f dir = p2 - p1;
    normalize(dir);
    sf::Vector2f right(dir.y, -dir.x);
    float texWidth = this->texture->getSize().x;
    float texHeight = this->texture->getSize().y;
    float scale = thickness / this->texture->getSize().y;

    Line line;
    sf::Vertex vertex;

    vertex.color = data.color;

    //Corner
    vertex.texCoords = sf::Vector2f(0, 0);
    vertex.position = p1 + (dir * -(texWidth * scale)) + (right * thickness / 2.f);
    line[0] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p1 + (right * thickness / 2.f);
    line[1] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p1 + (right * -thickness / 2.f);
    line[2] = vertex;

    vertex.texCoords = sf::Vector2f(0, texHeight);
    vertex.position = p1 + (dir * -(texWidth * scale)) + (right * -thickness / 2.f);
    line[3] = vertex;

    //middle
    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p1 + (right * thickness / 2.f);
    line[4] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p2 + (right * thickness / 2.f);
    line[5] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p2 + (right * -thickness / 2.f);
    line[6] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p1 + (right * -thickness / 2.f);
    line[7] = vertex;

    //Corner2
        //Corner
    vertex.texCoords = sf::Vector2f(0, 0);
    vertex.position = p2 + (dir * (texWidth * scale)) + (right * thickness / 2.f);
    line[8] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, 0);
    vertex.position = p2 + (right * thickness / 2.f);
    line[9] = vertex;

    vertex.texCoords = sf::Vector2f(texWidth, texHeight);
    vertex.position = p2 + (right * -thickness / 2.f);
    line[10] = vertex;

    vertex.texCoords = sf::Vector2f(0, texHeight);
    vertex.position = p2 + (dir * (texWidth * scale)) + (right * -thickness / 2.f);
    line[11] = vertex;

    lines.push_back(line);
}

void Tendril::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = this->texture;
    target.draw(vertices, states);
}
