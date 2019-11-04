#pragma once
#include "SFML/Graphics.hpp"
#include "Game/Collision/CollisionBox.h"
#include "Game/Collision/Explosion.h"

class Entity : public sf::Drawable
{
public:
    Entity(sf::Vector2f pos, sf::Texture * texture);
    ~Entity() {};

    void addCollision(CollisionBox::AABB aabb);


    //Uses textureRect to determinate bounds
    void addCollision();

    virtual void flipHorizontally();

    const CollisionBox& getCollisionBox() const { return this->collisionBox; };
    sf::Vector2f getSize() const { return this->size; };
    void setSize(sf::Vector2f size);
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const { return this->pos; };
    void setSpriteOffset(sf::Vector2f offset);
    sf::Vector2f getSpriteOffset() const { return this->spriteOffset; };
    void moveSpriteOffset(sf::Vector2f distance);
    sf::Vector2f getCenterPos() const;
    const sf::Texture* getTexture() const { return this->sprite.getTexture(); };

    //Might need to do more stuff if dimensions are changed
    void setTexture(const sf::Texture* texture) { this->sprite.setTexture(*texture); };
    void addCollisionComponent(CollisionBox::ColliderKeys component);

    float left() const;
    float right() const;
    float down() const;
    float up() const;
    float width() const;
    float height() const;

    virtual void handleCollision(const Entity* collider) = 0;
    virtual void handleExplosion(const Explosion& explosion) = 0;

protected:
    
    bool isFlippedHorizontally() const { return flipped; };
    void setTextureRect(sf::IntRect rect) { sprite.setTextureRect(rect); };
    void updateSpritePosition();
    
    CollisionBox collisionBox;
    sf::Vector2f pos;
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:

    sf::Vector2f spriteOffset;
    sf::Vector2f size;
    bool flipped;
    sf::Sprite sprite;
};