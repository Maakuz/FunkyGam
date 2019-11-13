#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include <set>
#include <string>

//Drawable is only for debug. Remove when sometime
class Collider : public sf::Drawable
{
public:
    enum ColliderKeys
    {
        ground = 501,
        platform = 502,
        levelWarp = 507,
        levelReturn = 508,
        customTerrain = 999,
        player = 1,
        Static = 2,
        throwable = 3,
        character = 4,
        grunt = 5,
        gatherable = 6,
        spell = 7
    };

    struct AABB
    {
        sf::Vector2f pos;
        sf::Vector2f size;

        AABB(sf::Vector2f pos, sf::Vector2f size);

        sf::Vector2f min() const
        {
            return this->pos;
        }

        sf::Vector2f max() const
        {
            return this->pos + this->size;
        }
    };

    static const int EDGE_SIZE = 16;

    Collider(AABB collisionBox, bool enabled = true);
    Collider(sf::Vector2f pos, sf::Vector2f size);
    ~Collider();

    bool intersects(const AABB& other) const;
    bool intersects(const Collider& other) const;
    static bool intersects(const AABB& a, const AABB& b);

    bool contains(const sf::Vector2f point) const;
    static bool contains(const AABB& aabb, sf::Vector2f point);

    void addComponent(ColliderKeys key);

    bool hasComponent(ColliderKeys key) const;

    AABB getAABB() const { return this->box; };
    void setAABB(AABB box);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setFlag(std::string flag) { this->flag = flag; };
    const std::string& getFlag()const { return flag; };

    Collider::AABB getUp() const { return this->up; };
    Collider::AABB getDown() const { return this->down; };
    Collider::AABB getLeft() const { return this->left; };
    Collider::AABB getRight() const { return this->right; };

    void enableCollision(bool boi) { this->enabled = boi; };
    bool isCollisionEnabled()const { return this->enabled; };

    static sf::Vector2f calculateCollisionForceOnObject(sf::Vector2f objPos, sf::Vector2f colliderPos, sf::Vector2f objMomentum, sf::Vector2f colliderMomentum, float objMass, float colliderMass);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    void setSides();

    bool enabled;
    AABB box;
    Collider::AABB up;
    Collider::AABB down;
    Collider::AABB left;
    Collider::AABB right;

    std::set<ColliderKeys> components;
    std::string flag;
};