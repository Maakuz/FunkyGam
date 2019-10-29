#pragma once
#include <string>
#include "Entities/Entity.h"
#include <fstream>

class Item : public Entity
{
public:
    Item(sf::Vector2f pos, sf::Texture* texture);
    virtual ~Item() {};

    friend std::istream& operator>>(std::istream& in, Item& item);

    int getID() const{ return id; };
    void setID(int id) { this->id = id; };

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

    int getStackLimit() const { return stackLimit; };
    void setStackLimit(int size) { this->stackLimit = size; };

    int getEmitterID() const { return emitterID; };
    void setEmitterID(int emitterID) { this->emitterID = emitterID; };

    bool isUseable()const { return useable; };
    void setUseable(bool useable) { this->useable = useable; };

    virtual void handleCollision(const Entity* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};

private:
    int id;
    std::string name;
    int stackLimit;
    int emitterID;
    bool useable;
};

inline Item::Item(sf::Vector2f pos, sf::Texture* texture) :
    Entity(pos, texture)
{
    id = 0;
    stackLimit = 0;
    emitterID = -1;
    useable = false;
}

inline std::istream& operator>>(std::istream& in, Item& item)
{
    std::string trash;
    in >> trash >> item.name;
    in >> trash >> item.stackLimit;
    in >> trash >> item.emitterID;
    in >> trash >> item.useable;
}