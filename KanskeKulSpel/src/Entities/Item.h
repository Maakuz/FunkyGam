#pragma once
#include <string>
#include "Entities/Entity.h"

class Item : public Entity
{
public:
    Item(sf::Vector2f pos, sf::Texture* texture);
    virtual ~Item() {};

    int getID() const{ return id; };
    void setID(int id) { this->id = id; };

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

    int getStackLimit() const { return stackLimit; };
    void setStackLimit(int size) { this->stackLimit = size; };

    virtual void handleCollision(const Entity* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};

private:
    int id;
    std::string name;
    int stackLimit;
};

inline Item::Item(sf::Vector2f pos, sf::Texture* texture) :
    Entity(pos, texture)
{
    id = 0;
    stackLimit = 0;
}