#pragma once
#include <string>
#include "Game/Entities/Entity.h"
#include <fstream>

class Item : public Entity
{
public:
    Item(sf::Vector2f pos, sf::Texture* texture);
    virtual ~Item() {};

    friend std::istream& operator>>(std::istream& in, Item& item);


    int getID() const { return id; };
    void setID(int id) { this->id = id; };

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

    int getStackLimit() const { return stackLimit; };
    void setStackLimit(int size) { this->stackLimit = size; };

    int getEmitterID() const { return emitterID; };
    void setEmitterID(int emitterID) { this->emitterID = emitterID; };

    bool isUseable()const { return useable; };
    void setUseable(bool useable) { this->useable = useable; };

    void pluck() { this->obtained = true; }; //I just wanted to name it like that it could have been called obtainItem or something like that but nonono
    bool isObtained() const { return obtained; };

    virtual void handleCollision(const Entity* collider) {};
    virtual void handleExplosion(const Explosion& explosion) {};

private:
    int id;
    std::string name;
    int stackLimit;
    int emitterID;
    bool useable;
    bool obtained;

    virtual std::istream& readSpecific(std::istream& in) { return in; };
};

