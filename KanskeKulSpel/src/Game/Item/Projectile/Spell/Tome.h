#pragma once
#include "Game/Entities/Entity.h"
#include "SFML/Graphics/Drawable.hpp"
#include "Game/Components/SpriteComp.h"
#include "Game/Components/LogisticsComp.h"

class Tome : public Entity, public sf::Drawable
{
public:
    Tome(sf::Vector2f pos, const sf::Texture* texture);
    virtual ~Tome() {};

    friend std::istream& operator>>(std::istream& in, Tome& tome);
    friend std::ostream& operator<<(std::ostream& out, const Tome& tome);

    std::string getSpell() const { return spell; };
    void setSpell(std::string spell) { this->spell = spell; };

    int getChannelEmitter() const { return channelEmitterID; };
    void setChannelEmitter(int emitterID) { this->channelEmitterID = emitterID; };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
    std::string spell;
    int channelEmitterID;
};