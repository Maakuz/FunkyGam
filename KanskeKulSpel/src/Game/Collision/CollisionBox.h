#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include <set>
#include <string>

//Rename, very confusing. Drawable is only for debug. Remove when sometime
class CollisionBox : public sf::Drawable
{
public:
    enum ColliderKeys
    {
        ground = 501,
        platform = 502,
        levelReturn = 508,
        customTerrain = 999,
        player = 1,
        Static = 2,
        throwable = 3,
        character = 4,
        grunt = 5,
        gatherable = 6,
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
    ~CollisionBox();

    bool intersects(const AABB& other) const;
    bool intersects(const AABB& a, const AABB& b) const;
    bool intersects(const CollisionBox& other) const;

    void addComponent(ColliderKeys key);

    bool hasComponent(ColliderKeys key) const;

    AABB getAABB() const { return this->box; };
    void setAABB(AABB box);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setFlag(std::string flag) { this->flag = flag; };
    const std::string& getFlag()const { return flag; };

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

    std::set<ColliderKeys> components;
    std::string flag;
};