#pragma once
#include "Game/Collision/Explosion.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "Comp.h"
#include <set>
#include <string>

enum class ColliderKeys
{
    ground,
    platform,
    levelWarp,
    levelReturn,
    customTerrain,
    player,
    enemy,
    Static,
    throwable,
    grunt,
    gatherable,
    fireball,
    hazard,
    projectilePassable,
    projectile
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


class ColliderComp : public Comp, public sf::Drawable
{
public:
    static const int EDGE_SIZE = 16;

    ColliderComp(sf::Vector2f size, const sf::Vector2f pos = sf::Vector2f());
    virtual ~ColliderComp() {};
    Comp* clone() const { return new ColliderComp(*this); };

     static ComponentKey getStaticKey() { return ComponentKey::collider; };

    void setPosition(sf::Vector2f pos);

    //adjusts the size if less than EDGE_SIZE and returns the difference
    sf::Vector2f adjustSize();

    sf::Vector2f getSize() const { return this->box.size; };
    void setSize(sf::Vector2f size);
    sf::Vector2f getCenterPos() const;

    float left() const;
    float right() const;
    float down() const;
    float up() const;
    float width() const;
    float height() const;

    bool intersects(const AABB& other) const;
    bool intersects(const ColliderComp& other) const;
    static bool intersects(const AABB& a, const AABB& b);

    bool contains(const sf::Vector2f point) const;
    static bool contains(const AABB& aabb, sf::Vector2f point);

    void addComponent(ColliderKeys key);

    bool hasComponent(ColliderKeys key) const;

    AABB getAABB() const { return this->box; };

    void setFlag(std::string flag) { this->flag = flag; };
    const std::string& getFlag()const { return flag; };

    const AABB& getUpBox() const { return this->upAABB; };
    const AABB& getDownBox() const { return this->downAABB; };
    const AABB& getLeftBox() const { return this->leftAABB; };
    const AABB& getRightBox() const { return this->rightAABB; };

    static sf::Vector2f calculateCollisionForceOnObject(sf::Vector2f objPos, sf::Vector2f colliderPos, sf::Vector2f objMomentum, sf::Vector2f colliderMomentum, float objMass, float colliderMass);



private:
    AABB box;
    AABB upAABB;
    AABB downAABB;
    AABB leftAABB;
    AABB rightAABB;

    std::set<ColliderKeys> components;
    std::string flag;

    void setSides();
    void setAABB(AABB box);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};