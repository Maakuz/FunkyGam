#pragma once
#include "Game/Components/SpriteComp.h"
#include "SFML/Graphics/VertexArray.hpp"
#include <vector>

class Chain : public sf::Drawable
{
public:
    struct Point
    {
        sf::Vector2f pos;
        sf::Vector2f prevPos;
        Point(sf::Vector2f pos = sf::Vector2f())
        {
            this->pos = pos;
            this->prevPos = pos;
        }
    };

    struct Link 
    {
        sf::Vertex v[4];
        const sf::Texture* texture;
        int p1;
        int p2;
        float halfWidth;
        float height;

        Link(const sf::Texture* tex)
        {
            p1 = 0;
            p2 = 0;
            texture = tex;

            halfWidth = tex->getSize().x / 2;
            height = tex->getSize().y;

            v[0].texCoords = sf::Vector2f(0, 0);
            v[1].texCoords = sf::Vector2f(halfWidth * 2.f, 0);
            v[2].texCoords = sf::Vector2f(halfWidth * 2.f, height);
            v[3].texCoords = sf::Vector2f(0, height);
        }

        sf::Vertex& operator[](int i)
        {
            return v[i];
        };
    };

    //If vector is provided the pieces will have
    Chain(const sf::Texture* texture, sf::Vector2f pos, int linkCount = 5, int stiffness = 10);
    Chain(std::vector<const sf::Texture*> textures, sf::Vector2f pos, int linkCount = 5, int stiffness = 10);
    virtual ~Chain(){};


    void update(float dt, sf::Vector2f mousePos);

private:
    std::vector<Link> links;
    std::vector<Point> points;
    sf::Vector2f pos;
    int stiffness;

    void move(Point* p);
    void constrainLink(Link* l);

    void initialize(std::vector<const sf::Texture*> textures, sf::Vector2f pos, int linkCount, float stiffness);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
};