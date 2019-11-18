#pragma once
#include "Game/Components/SpriteComp.h"
#include <vector>
#include "SFML/Graphics/VertexArray.hpp"

class Chain : public sf::Drawable
{
public:
    struct Link 
    {
        sf::Vertex v[4];
        const sf::Texture* texture;
        sf::Vector2f tetherPoint;
        sf::Vector2f nextTetherPoint;
        float angle;
        float mass;
        float halfWidth;
        float heigth;

        Link(const sf::Texture* tex)
        {
            texture = tex;
            angle = 0;
            mass = 0; 
            halfWidth = tex->getSize().x / 2;
            heigth = tex->getSize().y;

            v[0].texCoords = sf::Vector2f(0, 0);
            v[1].texCoords = sf::Vector2f(halfWidth * 2.f, 0);
            v[2].texCoords = sf::Vector2f(halfWidth * 2.f, heigth);
            v[3].texCoords = sf::Vector2f(0, heigth);
        }

        sf::Vertex& operator[](int i) 
        {
            return v[i];
        };
    };
    Chain(int linkCount = 5);
    virtual ~Chain(){};

    void reset(int linkCount = 5);

    void update(sf::Vector2f mousePos);

private:
    std::vector<Link> links;
    sf::Vector2f pos;
    sf::VertexArray arr;
    float stiffness;
    float gravity;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
};