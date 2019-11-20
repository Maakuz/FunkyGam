#include "Chain.h"
#include "Game/Misc/VectorFunctions.h"

Chain::Chain(const sf::Texture* texture, sf::Vector2f pos, int linkCount, int stiffness)
{
    std::vector<const sf::Texture*> tex;
    for (int i = 0; i < linkCount; i++)
    {
        tex.push_back(texture);
    }

    initialize(tex, pos, linkCount, stiffness);
}

Chain::Chain(std::vector<const sf::Texture*> textures, sf::Vector2f pos, int linkCount, int stiffness)
{
    initialize(textures, pos, linkCount, stiffness);
}

void Chain::setPos(sf::Vector2f pos, int point)
{
    if (point >= 0 && point < points.size())
        points[point].pos = pos;
}

void Chain::update(float dt)
{
    for (Point& p : points)
    {
        move(&p);
        //Constrain
    }

    for (int i = 0; i < this->stiffness; i++)
    {
        for (Link& l : links)
        {
            constrainLink(&l);
        }
    }

    for (Link& l : links)
    {
        sf::Vector2f dir(points[l.p2].pos - points[l.p1].pos);
        normalize(dir);
        dir = sf::Vector2f(dir.y, -dir.x);
 
         l[0].position = points[l.p1].pos - (dir * l.halfWidth);
 
         l[1].position = points[l.p1].pos + (dir * l.halfWidth);
 
         l[2].position = points[l.p2].pos + (dir * l.halfWidth);
 
         l[3].position = points[l.p2].pos - (dir * l.halfWidth);
    }

}

void Chain::move(Point* p)
{
    sf::Vector2f delta = p->pos - p->prevPos;
    delta *= 0.99f;//Include friction here
    p->prevPos = p->pos;
    p->pos += delta;
    p->pos.y += (mass * 0.3f); //Gravity
}

void Chain::constrainLink(Link* l)
{
    Point* p1 = &points[l->p1];
    Point* p2 = &points[l->p2];

    sf::Vector2f delta = p2->pos - p1->pos;
    float distance = length(delta);
    float frac = ((l->height - distance) / distance) / 2.f;

    delta *= frac;
    p1->pos -= delta;
    p2->pos += delta;
}

void Chain::initialize(std::vector<const sf::Texture*> textures, sf::Vector2f pos, int linkCount, float stiffness)
{
    links.clear();
    points.clear();
    this->stiffness = stiffness;
    this->mass = 1;


    points.push_back(Point(pos));

    for (int i = 0; i < linkCount; i++)
    {
        Link link(textures[i]);
        link.p1 = i;
        link.p2 = i + 1;

        points.push_back(Point(points.back().pos + sf::Vector2f(0, link.height)));

        link[0].position = points[link.p1].pos;
        link[0].position.x -= link.halfWidth;

        link[1].position = points[link.p1].pos;
        link[1].position.x += link.halfWidth;

        link[2].position = points[link.p2].pos;
        link[2].position.x += link.halfWidth;

        link[3].position = points[link.p2].pos;
        link[3].position.x -= link.halfWidth;
        
        links.push_back(link);
    }
}

void Chain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const Link& link : links)
    {
        states.texture = link.texture;
        target.draw(link.v, 4, sf::Quads, states);
    }
}