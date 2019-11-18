#include "Chain.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/VectorFunctions.h"
 
#define TESTTEX 12

Chain::Chain(int linkCount)
{
    pos = sf::Vector2f(700, 400);
    arr.setPrimitiveType(sf::LineStrip);
    reset(linkCount);
    stiffness = 5;
}

void Chain::reset(int linkCount)
{
    sf::Vertex v(sf::Vector2f(), sf::Color::Green);

    links.clear();
    for (int i = 0; i < linkCount; i++)
    {
        Link link(TextureHandler::get().getTexture(TESTTEX));

        link.mass = 1;

        if (links.empty())
        {
            link.tetherPoint = pos;
            link.tetherPoint.x += link.halfWidth;

            link.nextTetherPoint = link.tetherPoint;
            link.nextTetherPoint.y += link.heigth;
        }

        else
        {
            link.tetherPoint = links.back().nextTetherPoint;

            link.nextTetherPoint = link.tetherPoint;
            link.nextTetherPoint.y += link.heigth;
        }

        v.position = link.tetherPoint;
        arr.append(v);

        link[0].position = link.tetherPoint;
        link[0].position.x -= link.halfWidth;

        link[1].position = link.tetherPoint;
        link[1].position.x += link.halfWidth;

        link[2].position = link.nextTetherPoint;
        link[2].position.x += link.halfWidth;

        link[3].position = link.nextTetherPoint;
        link[3].position.x -= link.halfWidth;
        
        links.push_back(link);
    }
}

void Chain::update(sf::Vector2f mousePos)
{

    this->pos = mousePos;

    links.front().tetherPoint = this->pos;

    const Link* prevLink = nullptr;
    for (Link& link : links)
    {
        if (lengthSquared(link.nextTetherPoint - link.tetherPoint) > link.heigth * link.heigth)
            link.nextTetherPoint += (link.tetherPoint - link.nextTetherPoint) / stiffness;

        if (prevLink)
            link.tetherPoint = prevLink->nextTetherPoint;

        sf::Vector2f dir(link.tetherPoint - link.nextTetherPoint);
        normalize(dir);
        dir = sf::Vector2f(dir.y, -dir.x);

        link[0].position = link.tetherPoint - (dir * link.halfWidth);

        link[1].position = link.tetherPoint + (dir * link.halfWidth);

        link[2].position = link.nextTetherPoint + (dir * link.halfWidth);

        link[3].position = link.nextTetherPoint - (dir * link.halfWidth);

        prevLink = &link;
    }
}

void Chain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const Link& link : links)
    {
        states.texture = link.texture;
        target.draw(link.v, 4, sf::Quads, states);
    }

    states.texture = nullptr;

    target.draw(arr, states);
}