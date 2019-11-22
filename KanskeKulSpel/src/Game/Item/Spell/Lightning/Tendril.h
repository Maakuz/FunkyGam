#pragma once
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/Texture.hpp"

class Tendril : public sf::Drawable
{
public:
    Tendril(float thickness = 32, int splits = 10, int sway = 80, float peakHeight = 1);
    virtual ~Tendril() {};

    void generateLightning(sf::Vector2f start, sf::Vector2f end, float thickness, int splits);
    void generateLightningTree(sf::Vector2f start, sf::Vector2f end, float thickness, int splits, int min, int max, int angle);
private:
    friend class TendrilTester;

    sf::VertexArray lines;
    const sf::Texture* texture;
    sf::Color color;
    int sway;
    float peakHeight;

    void addLine(sf::Vector2f p1, sf::Vector2f p2, float thickness);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};