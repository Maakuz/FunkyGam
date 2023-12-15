#include "Tendril.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/UnorderedErase.h"
#include <istream>
#include <ostream>

std::ostream& operator<<(std::ostream& out, const Tendril::InitGenData& data)
{
    out << data.sway << " ";
    out << data.peakHeight << " ";
    out << data.thickness << " ";
    out << data.splits << " ";
    out << data.min << " ";
    out << data.max << " ";
    out << data.forkMin << " ";
    out << data.forkMax << " ";
    out << data.angle << " ";
    out << data.visibleTime << " ";
    out << data.fadeSpeed << " ";
    out << (int)data.color.r << " " << (int)data.color.g << " " << (int)data.color.b << " " << (int)data.color.a << " ";
    out << data.timeAlgorithm << " ";
    out << data.repeating << " ";

    return out;
};

std::istream& operator>>(std::istream& in, Tendril::InitGenData& data)
{
    int col[4] = { 0 };

    in >> data.sway;
    in >> data.peakHeight;
    in >> data.thickness;
    in >> data.splits;
    in >> data.min;
    in >> data.max;
    in >> data.forkMin;
    in >> data.forkMax;
    in >> data.angle;
    in >> data.visibleTime;
    in >> data.fadeSpeed;
    in >> col[0] >> col[1] >> col[2] >> col[3];
    data.color = sf::Color(col[0], col[1], col[2], col[3]);
    in >> data.timeAlgorithm;
    in >> data.repeating;

    return in;
};

Tendril::Tendril(InitGenData data)
{
    this->data = data;
    this->elapsedTime = 0;
    this->vertices.setPrimitiveType(sf::Quads);
    this->texture = TextureHandler::get().getTexture(28);
    this->complete = false;
}

Tendril::~Tendril()
{
}

void Tendril::update(float dt)
{

    elapsedTime += dt;
    if (data.visibleTime != 0)
    {
        float timePercentage = elapsedTime / data.visibleTime;
        for (Branch& branch : branches)
            updateBranch(&branch, timePercentage);
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
    if (elapsedTime >= data.visibleTime && lines.empty())
        this->complete = true;

    if (data.repeating && this->complete)
    {
        elapsedTime = 0;
        this->complete = false;

        for (Branch& b : branches)
            resetBranch(&b);
    }
}

void Tendril::resetBranch(Branch* branch)
{
    branch->progess = 0;
    for (Branch& b : branch->branches)
        resetBranch(&b);
}

void Tendril::updateBranch(Branch* branch, float time)
{
    int stop = branch->progess;

    if (stop < branch->distances.size())
    {
        for (int i = stop + 1; i < branch->distances.size() && branch->distances[i] < time; i++)
        {
            stop = i;
        }

        //Check if time is bigger than the biggest.
        if (branch->distances[stop] < time)
            stop++;

        for (int i = branch->progess; i < stop; i++)
        {
            addLine(branch->points[i], branch->points[i + 1], branch->thickness);
        }
    }

    for (int i = 0; i < branch->branchPositions.size() && branch->branchPositions[i] < stop; i++)
    {
        float localTime = branch->distances[branch->branchPositions[i]];

        switch (data.timeAlgorithm)
        {
        case 0:
            localTime = time;
            break;
        case 1:
            localTime = (time - localTime) / localTime;
            break;
        case 2:
            localTime = 1;
            break;
        case 3:
            break;

        default:
            localTime = time;
            break;
        }

        updateBranch(&branch->branches[i], localTime); //not quite right but hella cool
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
    int forks = min;
    if (max > min)
        forks = min + rand() % (max - min);

    branches->push_back(generateBranch(start, end, thickness, splits));

    int forkPosMax = std::max(std::min(data.forkMax, (int)branches->back().points.size()), 0);
    int forkPosMin = std::min(std::max(data.forkMin, 0), forkPosMax);

    if (forkPosMin == forkPosMax)
    {
        for (int i = 0; i < forks; i++)
            branches->back().branchPositions.push_back(std::min(forkPosMin, (int)branches->back().points.size() -1));
    }

    else
    {
        for (int i = 0; i < forks; i++)
            branches->back().branchPositions.push_back(forkPosMin + (rand() % (forkPosMax - forkPosMin)));
    }

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

std::ostream& operator<<(std::ostream& out, const Tendril& tendril)
{
    out << tendril.data;

    return out;
}

std::istream& operator>>(std::istream& in, Tendril& tendril)
{
    tendril.elapsedTime = 0;
    tendril.branches.clear();
    tendril.lines.clear();
    
    in >> tendril.data;

    return in;
}
