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

void Chain::initialize(std::vector<const sf::Texture*> textures, sf::Vector2f pos, int linkCount, float stiffness)
{
    links.clear();
    points.clear();
    this->stiffness = stiffness;
    this->mass = 1;
    this->seamless = true;

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


void Chain::setPos(sf::Vector2f pos, int point)
{
    if (point >= 0 && point < points.size())
        points[point].pos = pos;
}

void Chain::setTexture(const sf::Texture* texture, int link)
{
    this->links[link].setTexture(texture);
}

void Chain::update(float dt)
{
    for (int i = 0; i < points.size(); i++)
    {
        move(&points[i]);
        //Constrain
    }

    for (int i = 0; i < this->stiffness; i++)
    {
        for (Link& l : links)
        {
            constrainLink(&l);
        }
    }

    if (seamless)
    {
        sf::Vector2f prev;
        sf::Vector2f prevWidth;
        bool first = true;
        for (Link& l : links)
        {
            sf::Vector2f dir(points[l.p2].pos - points[l.p1].pos);
            normalize(dir);
            sf::Vector2f dirWidth = sf::Vector2f(dir.y, -dir.x);

            if (!first)
            {
                l[0].position = points[l.p1].pos - (prevWidth * (l.halfWidth + l.offset.x)) + (prev * l.offset.y);
                l[1].position = points[l.p1].pos + (prevWidth * (l.halfWidth - l.offset.x)) + (prev * l.offset.y);
            }

            else
            {
                l[0].position = points[l.p1].pos - (dirWidth * (l.halfWidth + l.offset.x)) + (prev * l.offset.y);
                l[1].position = points[l.p1].pos + (dirWidth * (l.halfWidth - l.offset.x)) + (prev * l.offset.y);
            }

            l[2].position = points[l.p2].pos + (dirWidth * (l.halfWidth - l.offset.x)) + (prev * l.offset.y);
            l[3].position = points[l.p2].pos - (dirWidth * (l.halfWidth + l.offset.x)) + (prev * l.offset.y);

            prev = dir;
            prevWidth = dirWidth;
            first = false;
        }
    }

    else
    {
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
}

void Chain::setLinkOffset(sf::Vector2f offset, int link)
{
    links[link].offset = offset;
}

void Chain::setLinkOffset(sf::Vector2f offset)
{
    for (Link& link : links)
        link.offset = offset;
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

void Chain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const Link& link : links)
    {
        states.texture = link.texture;
        target.draw(link.v, 4, sf::Quads, states);
    }
}