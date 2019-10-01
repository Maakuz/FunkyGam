#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include <set>

//Rename, very confusing. Drawable is only for debug. Remove when sometime
class CollisionBox : public sf::Drawable
{
public:
    enum colliderComponents
    {
        Ground = 501,
        Platform = 502,
        Player = 1,
        Static = 2,
        Bomb = 3
    };

    struct AABB
    {
        sf::Vector2f pos;
        sf::Vector2f size;
        
        AABB(sf::Vector2f pos, sf::Vector2f size)
        {
            this->pos = pos;
            this->size = size;
        }

        sf::Vector2f min() const
        {
            return this->pos;
        }

        sf::Vector2f max() const
        {
            return this->pos + this->size;
        }
    };


    CollisionBox(AABB collisionBox, bool enabled = true);
    CollisionBox(sf::Vector2f pos, sf::Vector2f size);
    ~CollisionBox() {};

    bool intersects(const AABB& other) const;
    bool intersects(const AABB& a, const AABB& b) const;
    bool intersects(const CollisionBox& other) const;

    void addComponent(colliderComponents comp);

    bool hasComponent(colliderComponents component) const;
    
    AABB getAABB() const { return this->box; };
    void setAABB(AABB box);
    void setPosition(sf::Vector2f pos);

    CollisionBox::AABB getUp() const { return this->up; };
    CollisionBox::AABB getDown() const { return this->down; };
    CollisionBox::AABB getLeft() const { return this->left; };
    CollisionBox::AABB getRight() const { return this->right; };

    void enableCollision(bool boi) { this->enabled = boi; };
    bool isCollisionEnabled()const { return this->enabled; };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    void setSides();

    bool enabled;
    AABB box;
    CollisionBox::AABB up;
    CollisionBox::AABB down;
    CollisionBox::AABB left;
    CollisionBox::AABB right;

    std::set<colliderComponents> components;
};