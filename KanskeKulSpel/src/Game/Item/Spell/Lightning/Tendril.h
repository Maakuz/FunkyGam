#pragma once
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/Texture.hpp"

class Tendril : public sf::Drawable
{
public:
    struct InitGenData
    {
        int sway;
        float peakHeight;
        float thickness;
        int splits;
        int min;
        int max;
        int angle;
        int visibleTime;
        float fadeSpeed;
        sf::Color color;

        InitGenData(float thickness = 32, int splits = 10, float fadeSpeed = 1, int min = 0, int max = 0, int sway = 80, float peakHeight = 1, float angle = 30, int visibleTime = 0)
        {
            this->sway = sway;
            this->thickness = thickness;
            this->splits = splits;
            this->peakHeight = peakHeight;
            this->min = min;
            this->max = max;
            this->angle = angle;
            this->visibleTime = visibleTime;
            this->fadeSpeed = fadeSpeed;
            color = sf::Color::White;
        }
    };

    Tendril(InitGenData data = InitGenData());
    virtual ~Tendril() {};

    void update(float dt);

    void generate(sf::Vector2f start, sf::Vector2f end);
private:
    struct Branch 
    {
        std::vector<sf::Vector2f> points;
        std::vector<float> distances;
        float thickness;
        std::vector<Branch> branches;
        std::vector<int> branchPositions;
        int progess;

        Branch()
        {
            this->progess = 0;
            this->thickness = 0;
        }
    };

    struct Line 
    {
        sf::Vertex vertices[12];

        sf::Vertex& operator[](int i) 
        {
            return vertices[i];
        }
    };

    friend class TendrilTester;
    friend class Emitter;

    float elapsedTime;
    InitGenData data;
    std::vector<Branch> branches;
    std::vector<Line> lines;
    sf::VertexArray vertices;
    const sf::Texture* texture;

    void updateBranch(Branch* branch, float time);
    void generateLightning(sf::Vector2f start, sf::Vector2f end);
    void generateLightningTree(sf::Vector2f start, sf::Vector2f end);
    void generateLightningTreeRec(std::vector<Branch>* branches, sf::Vector2f start, sf::Vector2f end, float thickness, int splits, int min, int max, int angle);
    Branch generateBranch(sf::Vector2f start, sf::Vector2f end, float thickness, int splits);


    void addLine(sf::Vector2f p1, sf::Vector2f p2, float thickness);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};