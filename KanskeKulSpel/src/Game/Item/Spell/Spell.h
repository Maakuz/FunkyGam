#pragma once
#include "Game/Entities/Entity.h"

class Spell : public Entity
{
public:
    Spell(sf::Vector2f pos);
    virtual bool isComplete() const = 0;
    virtual void cast(sf::Vector2f pos, sf::Vector2f dest) = 0;
    virtual void update(float dt) = 0;

    virtual std::istream& readSpecific(std::istream& in) =0;
    virtual std::ostream& writeSpecific(std::ostream& out)const=0;

    friend std::istream& operator>>(std::istream& in, Spell& spell);
    friend std::ostream& operator<<(std::ostream& out, const Spell& spell);

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

private:
    std::string name;
};