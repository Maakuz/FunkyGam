#pragma once
#include "Game/Item/Item.h"

class Tome : public Item
{
public:
    Tome(sf::Vector2f pos, const sf::Texture* texture);
    ~Tome() {};

    virtual std::istream& readSpecific(std::istream& in);
    virtual std::ostream& writeSpecific(std::ostream& out)const;

    std::string getSpell() const { return spell; };
    void setSpell(std::string spell) { this->spell = spell; };

    int getChannelEmitter() const { return channelEmitterID; };
    void setChannelEmitter(int emitterID) { this->channelEmitterID = emitterID; };

private:
    std::string spell;
    int channelEmitterID;
};