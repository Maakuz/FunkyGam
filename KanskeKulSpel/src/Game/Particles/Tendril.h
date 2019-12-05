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
        int forkMin;
        int forkMax;
        int angle;
        int visibleTime;
        float fadeSpeed;
        sf::Color color;
        int timeAlgorithm;
        bool repeating;

        InitGenData()
        {
            this->sway = 80;
            this->thickness = 32;
            this->splits = 10;
            this->peakHeight = 1;
            this->min = 0;
            this->max = 0;
            this->angle = 30;
            this->visibleTime = 0;
            this->fadeSpeed = 1;
            this->color = sf::Color::White;
            this->forkMin = 0;
            this->forkMax = splits;
            this->timeAlgorithm = 0;
            repeating = false;
        }

        friend std::ostream& operator<<(std::ostream& out, const InitGenData& data);

        friend std::istream& operator>>(std::istream& in, InitGenData& data);
    };

    Tendril(InitGenData data = InitGenData());
    virtual ~Tendril() {};

    friend std::ostream& operator<<(std::ostream& out, const Tendril& tendril);
    friend std::istream& operator>>(std::istream& in, Tendril& tendril);

    void update(float dt);
    void generate(sf::Vector2f start, sf::Vector2f end);
    InitGenData* getDataPtr() { return &data; };
    bool isComplete()const { return this->complete; };
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

    float elapsedTime;
    InitGenData data;
    std::vector<Branch> branches;
    std::vector<Line> lines;
    sf::VertexArray vertices;
    const sf::Texture* texture;
    bool complete;

    void resetBranch(Branch* branch);

    void updateBranch(Branch* branch, float time);
    void generateLightning(sf::Vector2f start, sf::Vector2f end);
    void generateLightningTree(sf::Vector2f start, sf::Vector2f end);
    void generateLightningTreeRec(std::vector<Branch>* branches, sf::Vector2f start, sf::Vector2f end, float thickness, int splits, int min, int max, int angle);
    Branch generateBranch(sf::Vector2f start, sf::Vector2f end, float thickness, int splits);


    void addLine(sf::Vector2f p1, sf::Vector2f p2, float thickness);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};