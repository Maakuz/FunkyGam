#pragma once
#include <istream>
#include <ostream>
#include <string>
#include "Game/Components/TransformComp.h"
#include "Game/Entities/Collidable.h"

class Spell
{
public:
    Spell(sf::Vector2f pos);
    virtual ~Spell() {};
    virtual bool isComplete() const = 0;
    virtual void cast(sf::Vector2f pos, sf::Vector2f dest, float channelTime) = 0;
    virtual void update(float dt) = 0;

    virtual std::istream& readSpecific(std::istream& in) =0;
    virtual std::ostream& writeSpecific(std::ostream& out)const=0;

    friend std::istream& operator>>(std::istream& in, Spell& spell);
    friend std::ostream& operator<<(std::ostream& out, const Spell& spell);

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

protected:
    TransformComp transform;

private:
    std::string name;
};